/* corefile.h

   Copyright 2000, 2001 Free Software Foundation, Inc.

This file is part of GNU Binutils.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef corefile_h
#define corefile_h

#include "bfd.h"

extern bfd *core_bfd;		/* BFD for core-file.  */
extern int core_num_syms;	/* # of entries in symbol-table.  */
extern asymbol **core_syms;	/* Symbol table in a.out.  */
extern asection *core_text_sect;/* Core text section.  */
extern PTR core_text_space;	/* Text space of a.out in core.  */
extern int min_insn_size;	/* Size of smallest instruction, in bytes.  */
extern int offset_to_code;	/* Offset (in bytes) of code from entry
				   address of routine.  */

extern void core_init                  PARAMS ((const char *));
extern void core_get_text_space        PARAMS ((bfd *));
extern void core_create_function_syms  PARAMS ((bfd *));
extern void core_create_line_syms      PARAMS ((bfd *));

#endif /* corefile_h */
