/*	$OpenBSD: ufs_inode.c,v 1.15.2.4 2003/05/19 22:38:11 tedu Exp $	*/
/*	$NetBSD: ufs_inode.c,v 1.7 1996/05/11 18:27:52 mycroft Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ufs_inode.c	8.7 (Berkeley) 7/22/94
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/namei.h>

#include <ufs/ufs/extattr.h>
#include <ufs/ufs/quota.h>
#include <ufs/ufs/inode.h>
#include <ufs/ufs/ufsmount.h>
#include <ufs/ufs/ufs_extern.h>

u_long	nextgennumber;		/* Next generation number to assign. */

#if 0
void
ufs_init()
{
	static int done = 0;

	if (done)
		return;
	done = 1;
	ufs_ihashinit();
	ufs_quota_init();

	return;
}
#endif
/*
 * Last reference to an inode.  If necessary, write or delete it.
 */
int
ufs_inactive(v)
	void *v;
{
	struct vop_inactive_args /* {
		struct vnode *a_vp;
		struct proc *a_p;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;
	struct inode *ip = VTOI(vp);
	struct proc *p = ap->a_p;
	int mode, error = 0;
	extern int prtactive;

	if (prtactive && vp->v_usecount != 0)
		vprint("ufs_inactive: pushing active", vp);

	/*
	 * Ignore inodes related to stale file handles.
	 */
	if (ip->i_ffs_mode == 0)
		goto out;
	if (ip->i_ffs_nlink <= 0 && (vp->v_mount->mnt_flag & MNT_RDONLY) == 0) {
		if (getinoquota(ip) == 0)
			(void)ufs_quota_free_inode(ip, NOCRED);

		if (ip->i_ffs_size != 0) {
			(void) UFS_TRUNCATE(ip, (off_t)0, 0, NOCRED);
		}
		ip->i_ffs_rdev = 0;
		mode = ip->i_ffs_mode;
		ip->i_ffs_mode = 0;
		ip->i_flag |= IN_CHANGE | IN_UPDATE;
		UFS_INODE_FREE(ip, ip->i_number, mode);
	}
	if (ip->i_flag & (IN_ACCESS | IN_CHANGE | IN_MODIFIED | IN_UPDATE)) {
		UFS_UPDATE(ip, 0);
	}
out:
	VOP_UNLOCK(vp, 0, p);
	/*
	 * If we are done with the inode, reclaim it
	 * so that it can be reused immediately.
	 */
	if (ip->i_ffs_mode == 0)
		vrecycle(vp, NULL, p);
	return (error);
}

/*
 * Reclaim an inode so that it can be used for other purposes.
 */
int
ufs_reclaim(vp, p)
	struct vnode *vp;
	struct proc *p;
{
	struct inode *ip;
	extern int prtactive;

	if (prtactive && vp->v_usecount != 0)
		vprint("ufs_reclaim: pushing active", vp);
	/*
	 * Remove the inode from its hash chain.
	 */
	ip = VTOI(vp);
	ufs_ihashrem(ip);
	/*
	 * Purge old data structures associated with the inode.
	 */
	cache_purge(vp);
	if (ip->i_devvp) {
		vrele(ip->i_devvp);
		ip->i_devvp = 0;
	}
	ufs_quota_delete(ip);
	return (0);
}

/*
 * allocate a range of blocks in a file.
 * after this function returns, any page entirely contained within the range
 * will map to invalid data and thus must be overwritten before it is made
 * accessible to others.
 */

int
ufs_balloc_range(vp, off, len, cred, flags)
	struct vnode *vp;
	off_t off, len;
	struct ucred *cred;
	int flags;
{
	off_t oldeof, neweof, oldeob, neweob, pagestart;
	struct uvm_object *uobj;
	struct genfs_node *gp = VTOG(vp);
	int i, delta, error, npages;
	int bshift = vp->v_mount->mnt_fs_bshift;
	int bsize = 1 << bshift;
	int ppb = MAX(bsize >> PAGE_SHIFT, 1);
	struct vm_page *pgs[ppb];
	UVMHIST_FUNC("ufs_balloc_range"); UVMHIST_CALLED(ubchist);
	UVMHIST_LOG(ubchist, "vp %p off 0x%x len 0x%x u_size 0x%x",
		    vp, off, len, vp->v_size);

	oldeof = vp->v_size;
	GOP_SIZE(vp, oldeof, &oldeob);

	neweof = MAX(vp->v_size, off + len);
	GOP_SIZE(vp, neweof, &neweob);

	error = 0;
	uobj = &vp->v_uobj;
	pgs[0] = NULL;

	/*
	 * read or create pages covering the range of the allocation and
	 * keep them locked until the new block is allocated, so there
	 * will be no window where the old contents of the new block are
	 * visible to racing threads.
	 */

	pagestart = trunc_page(off) & ~(bsize - 1);
	npages = MIN(ppb, (round_page(neweob) - pagestart) >> PAGE_SHIFT);
	memset(pgs, 0, npages * sizeof(struct vm_page *));
	simple_lock(&uobj->vmobjlock);
	error = VOP_GETPAGES(vp, pagestart, pgs, &npages, 0,
	    VM_PROT_READ, 0, PGO_SYNCIO|PGO_PASTEOF);
	if (error) {
		return error;
	}
	simple_lock(&uobj->vmobjlock);
	uvm_lock_pageq();
	for (i = 0; i < npages; i++) {
		UVMHIST_LOG(ubchist, "got pgs[%d] %p", i, pgs[i],0,0);
		KASSERT((pgs[i]->flags & PG_RELEASED) == 0);
		pgs[i]->flags &= ~PG_CLEAN;
		uvm_pageactivate(pgs[i]);
	}
	uvm_unlock_pageq();
	simple_unlock(&uobj->vmobjlock);

	/*
	 * adjust off to be block-aligned.
	 */

	delta = off & (bsize - 1);
	off -= delta;
	len += delta;

	/*
	 * now allocate the range.
	 */

	lockmgr(&gp->g_glock, LK_EXCLUSIVE, NULL, curproc);
	error = GOP_ALLOC(vp, off, len, flags, cred);
	lockmgr(&gp->g_glock, LK_RELEASE, NULL, curproc);

	/*
	 * clear PG_RDONLY on any pages we are holding
	 * (since they now have backing store) and unbusy them.
	 */

	simple_lock(&uobj->vmobjlock);
	for (i = 0; i < npages; i++) {
		pgs[i]->flags &= ~PG_RDONLY;
		if (error) {
			pgs[i]->flags |= PG_RELEASED;
		}
	}
	if (error) {
		uvm_lock_pageq();
		uvm_page_unbusy(pgs, npages);
		uvm_unlock_pageq();
	} else {
		uvm_page_unbusy(pgs, npages);
	}
	simple_unlock(&uobj->vmobjlock);
	return error;
}
