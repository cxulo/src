/*	$OpenBSD: rtld_machine.c,v 1.3 2001/09/25 14:29:48 art Exp $ */

/*
 * Copyright (c) 1999 Dale Rahn
 * Copyright (c) 2001 Niklas Hallqvist
 * Copyright (c) 2001 Artur Grabowski
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
 *	This product includes software developed under OpenBSD by
 *	Dale Rahn.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*-
 * Copyright (c) 2000 Eduardo Horvath.
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#define _DYN_LOADER

#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/mman.h>

#include <nlist.h>
#include <link.h>

#include "syscall.h"
#include "archdep.h"
#include "resolve.h"

/*
 * The following table holds for each relocation type:
 *	- the width in bits of the memory location the relocation
 *	  applies to (not currently used)
 *	- the number of bits the relocation value must be shifted to the
 *	  right (i.e. discard least significant bits) to fit into
 *	  the appropriate field in the instruction word.
 *	- flags indicating whether
 *		* the relocation involves a symbol
 *		* the relocation is relative to the current position
 *		* the relocation is for a GOT entry
 *		* the relocation is relative to the load address
 *
 */
#define _RF_S		0x80000000		/* Resolve symbol */
#define _RF_A		0x40000000		/* Use addend */
#define _RF_P		0x20000000		/* Location relative */
#define _RF_G		0x10000000		/* GOT offset */
#define _RF_B		0x08000000		/* Load address relative */
#define _RF_U		0x04000000		/* Unaligned */
#define _RF_SZ(s)	(((s) & 0xff) << 8)	/* memory target size */
#define _RF_RS(s)	( (s) & 0xff)		/* right shift */
static int reloc_target_flags[] = {
	0,							/* NONE */
	_RF_S|_RF_A|		_RF_SZ(8)  | _RF_RS(0),		/* RELOC_8 */
	_RF_S|_RF_A|		_RF_SZ(16) | _RF_RS(0),		/* RELOC_16 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* RELOC_32 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(8)  | _RF_RS(0),		/* DISP_8 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(16) | _RF_RS(0),		/* DISP_16 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(0),		/* DISP_32 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(2),		/* WDISP_30 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(2),		/* WDISP_22 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(10),	/* HI22 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 22 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 13 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* LO10 */
	_RF_G|			_RF_SZ(32) | _RF_RS(0),		/* GOT10 */
	_RF_G|			_RF_SZ(32) | _RF_RS(0),		/* GOT13 */
	_RF_G|			_RF_SZ(32) | _RF_RS(10),	/* GOT22 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(0),		/* PC10 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(10),	/* PC22 */
	      _RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(2),		/* WPLT30 */
				_RF_SZ(32) | _RF_RS(0),		/* COPY */
	_RF_S|_RF_A|		_RF_SZ(64) | _RF_RS(0),		/* GLOB_DAT */
				_RF_SZ(32) | _RF_RS(0),		/* JMP_SLOT */
	      _RF_A|	_RF_B|	_RF_SZ(64) | _RF_RS(0),		/* RELATIVE */
	_RF_S|_RF_A|	_RF_U|	_RF_SZ(32) | _RF_RS(0),		/* UA_32 */

	      _RF_A|		_RF_SZ(32) | _RF_RS(0),		/* PLT32 */
	      _RF_A|		_RF_SZ(32) | _RF_RS(10),	/* HIPLT22 */
	      _RF_A|		_RF_SZ(32) | _RF_RS(0),		/* LOPLT10 */
	      _RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(0),		/* PCPLT32 */
	      _RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(10),	/* PCPLT22 */
	      _RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(0),		/* PCPLT10 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 10 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 11 */
	_RF_S|_RF_A|		_RF_SZ(64) | _RF_RS(0),		/* 64 */
	_RF_S|_RF_A|/*extra*/	_RF_SZ(32) | _RF_RS(0),		/* OLO10 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(42),	/* HH22 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(32),	/* HM10 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(10),	/* LM22 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(42),	/* PC_HH22 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(32),	/* PC_HM10 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(10),	/* PC_LM22 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(2),		/* WDISP16 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(32) | _RF_RS(2),		/* WDISP19 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* GLOB_JMP */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 7 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 5 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* 6 */
	_RF_S|_RF_A|_RF_P|	_RF_SZ(64) | _RF_RS(0),		/* DISP64 */
	      _RF_A|		_RF_SZ(64) | _RF_RS(0),		/* PLT64 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(10),	/* HIX22 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* LOX10 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(22),	/* H44 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(12),	/* M44 */
	_RF_S|_RF_A|		_RF_SZ(32) | _RF_RS(0),		/* L44 */
	_RF_S|_RF_A|		_RF_SZ(64) | _RF_RS(0),		/* REGISTER */
	_RF_S|_RF_A|	_RF_U|	_RF_SZ(64) | _RF_RS(0),		/* UA64 */
	_RF_S|_RF_A|	_RF_U|	_RF_SZ(16) | _RF_RS(0),		/* UA16 */
};

#define RELOC_RESOLVE_SYMBOL(t)		((reloc_target_flags[t] & _RF_S) != 0)
#define RELOC_PC_RELATIVE(t)		((reloc_target_flags[t] & _RF_P) != 0)
#define RELOC_BASE_RELATIVE(t)		((reloc_target_flags[t] & _RF_B) != 0)
#define RELOC_UNALIGNED(t)		((reloc_target_flags[t] & _RF_U) != 0)
#define RELOC_USE_ADDEND(t)		((reloc_target_flags[t] & _RF_A) != 0)
#define RELOC_TARGET_SIZE(t)		((reloc_target_flags[t] >> 8) & 0xff)
#define RELOC_VALUE_RIGHTSHIFT(t)	(reloc_target_flags[t] & 0xff)

static long reloc_target_bitmask[] = {
#define _BM(x)	(~(-(1ULL << (x))))
	0,				/* NONE */
	_BM(8), _BM(16), _BM(32),	/* RELOC_8, _16, _32 */
	_BM(8), _BM(16), _BM(32),	/* DISP8, DISP16, DISP32 */
	_BM(30), _BM(22),		/* WDISP30, WDISP22 */
	_BM(22), _BM(22),		/* HI22, _22 */
	_BM(13), _BM(10),		/* RELOC_13, _LO10 */
	_BM(10), _BM(13), _BM(22),	/* GOT10, GOT13, GOT22 */
	_BM(10), _BM(22),		/* _PC10, _PC22 */  
	_BM(30), 0,			/* _WPLT30, _COPY */
	_BM(32), _BM(32), _BM(32),	/* _GLOB_DAT, JMP_SLOT, _RELATIVE */
	_BM(32), _BM(32),		/* _UA32, PLT32 */
	_BM(22), _BM(10),		/* _HIPLT22, LOPLT10 */
	_BM(32), _BM(22), _BM(10),	/* _PCPLT32, _PCPLT22, _PCPLT10 */
	_BM(10), _BM(11), -1,		/* _10, _11, _64 */
	_BM(10), _BM(22),		/* _OLO10, _HH22 */
	_BM(10), _BM(22),		/* _HM10, _LM22 */
	_BM(22), _BM(10), _BM(22),	/* _PC_HH22, _PC_HM10, _PC_LM22 */
	_BM(16), _BM(19),		/* _WDISP16, _WDISP19 */
	-1,				/* GLOB_JMP */
	_BM(7), _BM(5), _BM(6)		/* _7, _5, _6 */
	-1, -1,				/* DISP64, PLT64 */
	_BM(22), _BM(13),		/* HIX22, LOX10 */
	_BM(22), _BM(10), _BM(13),	/* H44, M44, L44 */
	-1, -1, _BM(16),		/* REGISTER, UA64, UA16 */
#undef _BM
};
#define RELOC_VALUE_BITMASK(t)	(reloc_target_bitmask[t])

int
_dl_md_reloc(elf_object_t *object, int rel, int relasz)
{
	long	i;
	long	numrela;
	long	fails = 0;
	Elf_Addr loff, mask;
	Elf_RelA *relas;
	load_list_t *llist;

	loff   = object->load_offs;
	numrela = object->Dyn.info[relasz] / sizeof(Elf64_Rela);
	relas = (Elf64_Rela *)(object->Dyn.info[rel]);

	if ((object->status & STAT_RELOC_DONE) || !relas) {
		return(0);
	}

	/*
	 * unprotect some segments if we need it.
	 * XXX - we unprotect waay to much. only the text can have cow
	 *       relocations.
	 */
	if ((rel == DT_REL || rel == DT_RELA)) {
		for (llist = object->load_list; llist != NULL; llist = llist->next) {
			if (!(llist->prot & PROT_WRITE)) {
				_dl_mprotect(llist->start, llist->size,
					llist->prot|PROT_WRITE);
			}
		}
	}

	for (i = 0; i < numrela; i++, relas++) {
		Elf_Addr *where, value, ooff;
		Elf_Word type;
		const Elf_Sym *sym, *this;
		const char *symn;

		where = (Elf64_Addr *)(relas->r_offset + loff);

		type = ELF_R_TYPE(relas->r_info);
		value = relas->r_addend;

		if (type == R_TYPE(NONE))
			continue;

		if (type == R_TYPE(JMP_SLOT)) {
_dl_printf("JMP_SLOT relocation\n");
			continue;
		}

		if (type == R_TYPE(COPY)) {
_dl_printf("COPY relocation\n");
			continue;
		}

		if (type == R_TYPE(RELATIVE)) {
			*where = (Elf_Addr)(loff + value);
			continue;
		}

		if (RELOC_RESOLVE_SYMBOL(type)) {
			sym = object->dyn.symtab;
			sym += ELF_R_SYM(relas->r_info);
			symn = object->dyn.strtab + sym->st_name;

			this = NULL;
			ooff = _dl_find_symbol(symn, _dl_objects, &this, 0, 1);
			if (this == NULL) {
				_dl_printf("%s: %s :can't resolve reference '%s'\n",
					_dl_progname, object->load_name, symn);
				fails++;
				continue;
			}

			value += (Elf_Addr)(ooff + this->st_value);
		}

		if (RELOC_PC_RELATIVE(type)) {
			value -= (Elf_Addr)where;
		}

		if (RELOC_BASE_RELATIVE(type)) {
			value += loff;
		}

		if (RELOC_UNALIGNED(type)) {
			/* Handle unaligned relocations. */
			Elf_Addr tmp = 0;
			char *ptr = (char *)where;
			int i, size = RELOC_TARGET_SIZE(type)/8;
		
			/* Read it in one byte at a time. */
			for (i=0; i<size; i++)
				tmp = (tmp << 8) | ptr[i];

			tmp &= ~mask;
			tmp |= value;

			/* Write it back out. */
			for (i=0; i<size; i++)
				ptr[i] = ((tmp >> (8*i)) & 0xff);
		} else if (RELOC_TARGET_SIZE(type) > 32) {
			*where &= ~mask;
			*where |= value;
		} else {
			Elf32_Addr *where32 = (Elf32_Addr *)where;

			*where32 &= ~mask;
			*where32 |= value;
		}
	}
	/*
	 * flush ??  - XXX
	__asm __volatile("imb" : : : "memory");
	 */

	/* reprotect the unprotected segments */
	if ((rel == DT_REL || rel == DT_RELA)) {
		for (llist = object->load_list; llist != NULL; llist = llist->next) {
			if (!(llist->prot & PROT_WRITE)) {
				_dl_mprotect(llist->start, llist->size,
					llist->prot);
			}
		}
	}

	return (fails);
}

/*
 * Resolve a symbol at run-time.
 */
void *
_dl_bind(elf_object_t *object, Elf_Word reloff)
{
	Elf_RelA *rela;
	Elf_Addr *addr, ooff;
	const Elf_Sym *sym, *this;
	const char *symn;

	rela = (Elf_RelA *)(object->Dyn.info[DT_JMPREL] + reloff);

	sym = object->dyn.symtab;
	sym += ELF64_R_SYM(rela->r_info);
	symn = object->dyn.strtab + sym->st_name;

	addr = (Elf_Addr *)(object->load_offs + rela->r_offset);
	ooff = _dl_find_symbol(symn, _dl_objects, &this, 0, 1);
	if (this == NULL) {
		_dl_printf("lazy binding failed!\n");
		*((int *)0) = 0;	/* XXX */
	}
	*addr = ooff + this->st_value + rela->r_addend;

	return (void *)*addr;
}

/*
 *	Relocate the Global Offset Table (GOT).
 */
void
_dl_md_reloc_got(elf_object_t *object, int lazy)
{
	Elf_Addr *pltgot;
	extern void _dl_bind_start(void);	/* XXX */

	lazy = 0; /* not yet */
	pltgot = (Elf_Addr *)object->Dyn.info[DT_PLTGOT];

	if (object->obj_type == OBJTYPE_LDR || !lazy || pltgot == NULL) {
		_dl_md_reloc(object, DT_JMPREL, DT_PLTRELSZ);
		return;
	}

#ifdef SUPPORT_LAZY
	if (object->obj_type != OBJTYPE_EXE) {
		int i, size;
		Elf_Addr *addr;
		Elf_RelA *rela;

		size = object->Dyn.info[DT_PLTRELSZ] / sizeof(Elf_RelA);
		rela = (Elf_RelA *)(object->Dyn.info[DT_JMPREL]);

		for (i = 0; i < size; i++) {
			addr = (Elf_Addr *)(object->load_offs + rela[i].r_offset);
			*addr += object->load_offs;
		}
	}

	pltgot[2] = (Elf_Addr)_dl_bind_start;
	pltgot[3] = (Elf_Addr)object;
#endif /* SUPPORT_LAZY */
}
