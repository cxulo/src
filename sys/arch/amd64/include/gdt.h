/*	$OpenBSD: gdt.h,v 1.4 2009/02/03 11:24:19 mikeb Exp $	*/
/*	$NetBSD: gdt.h,v 1.1 2003/04/26 18:39:40 fvdl Exp $	*/

/*-
 * Copyright (c) 1996, 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by John T. Kohl and Charles M. Hannum.
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

#ifndef _LOCORE
struct proc;
struct pmap;

void gdt_init(void);
void gdt_init_cpu(struct cpu_info *);
void gdt_reload_cpu(struct cpu_info *);
void gdt_alloc_cpu(struct cpu_info *);

int tss_alloc(struct pcb *);
void tss_free(int);

void ldt_alloc(struct pmap *, char *, size_t);
void ldt_free(struct pmap *);

void set_mem_gdt(struct mem_segment_descriptor *, void *, size_t,
		 int, int, int, int, int);
void set_sys_gdt(struct sys_segment_descriptor *, void *, size_t, int, int,
		 int);
#endif

/*
 * Maximum GDT size.  It cannot exceed 65536 since the selector field of
 * a descriptor is just 16 bits, and used as free list link.
 */
#define MAXGDTSIZ       65536