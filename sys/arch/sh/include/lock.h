/*	$OpenBSD: lock.h,v 1.3 2010/04/26 05:48:19 deraadt Exp $	*/
/*	$NetBSD: lock.h,v 1.10 2006/01/03 01:29:46 uwe Exp $	*/

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Gregory McGarry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Machine-dependent spin lock operations.
 */

#ifndef _SH_LOCK_H_
#define	_SH_LOCK_H_

#include <sh/atomic.h>

typedef volatile u_int8_t __cpu_simple_lock_t;

#define	__SIMPLELOCK_LOCKED	0x80
#define	__SIMPLELOCK_UNLOCKED	0x00

static __inline void
__cpu_simple_lock_init(__cpu_simple_lock_t *alp)
{
	*alp = __SIMPLELOCK_UNLOCKED;
}

static __inline void
__cpu_simple_lock(__cpu_simple_lock_t *alp)
{
	 __asm volatile(
		"1:	tas.b	%0	\n"
		"	bf	1b	\n"
		: "=m" (*alp));
}

static __inline int
__cpu_simple_lock_try(__cpu_simple_lock_t *alp)
{
	int __rv;

	__asm volatile(
		"	tas.b	%0	\n"
		"	mov	#0, %1	\n"
		"	rotcl	%1	\n"
		: "=m" (*alp), "=r" (__rv));

	return (__rv);
}

static __inline void
__cpu_simple_unlock(__cpu_simple_lock_t *alp)
{
	*alp = __SIMPLELOCK_UNLOCKED;
}

#endif /* !_SH_LOCK_H_ */
