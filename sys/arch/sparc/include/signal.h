/*	$OpenBSD: signal.h,v 1.3 1997/08/08 08:26:48 downsj Exp $	*/
/*	$NetBSD: signal.h,v 1.4 1996/02/01 22:32:35 mycroft Exp $ */

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	@(#)signal.h	8.1 (Berkeley) 6/11/93
 */

#ifndef	_SPARC_SIGNAL_H_
#define _SPARC_SIGNAL_H_

#ifndef _LOCORE
typedef int sig_atomic_t;
#endif

#ifndef _ANSI_SOURCE
#ifndef _LOCORE

/*
 * Information pushed on stack when a signal is delivered.
 * This is used by the kernel to restore state following
 * execution of the signal handler.  It is also made available
 * to the handler to allow it to restore state properly if
 * a non-standard exit is performed.
 *
 * All machines must have an sc_onstack and sc_mask.
 */
struct sigcontext {
	int	sc_onstack;		/* sigstack state to restore */
	int	sc_mask;		/* signal mask to restore */
	/* begin machine dependent portion */
	int	sc_sp;			/* %sp to restore */
	int	sc_pc;			/* pc to restore */
	int	sc_npc;			/* npc to restore */
	int	sc_psr;			/* psr to restore */
	int	sc_g1;			/* %g1 to restore */
	int	sc_o0;			/* %o0 to restore */
};
#else /* _LOCORE */
#define	SC_SP_OFFSET	8
#define	SC_PC_OFFSET	12
#define	SC_NPC_OFFSET	16
#define	SC_PSR_OFFSET	20
#define	SC_G1_OFFSET	24
#define	SC_O0_OFFSET	28
#endif /* _LOCORE */

/*
 * `Code' arguments to signal handlers.  The names, and the funny numbering.
 * are defined so as to match up with what SunOS uses; I have no idea why
 * they did the numbers that way, except maybe to match up with the 68881.
 */
#define	FPE_INTOVF_TRAP		0x01	/* integer overflow */
#define	FPE_INTDIV_TRAP		0x14	/* integer divide by zero */
#define	FPE_FLTINEX_TRAP	0xc4	/* inexact */
#define	FPE_FLTDIV_TRAP		0xc8	/* divide by zero */
#define	FPE_FLTUND_TRAP		0xcc	/* underflow */
#define	FPE_FLTOPERR_TRAP	0xd0	/* operand error */
#define	FPE_FLTOVF_TRAP		0xd4	/* overflow */

#endif	/* !_ANSI_SOURCE */
#endif	/* !_SPARC_SIGNAL_H_ */
