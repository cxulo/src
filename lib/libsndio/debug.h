/*	$OpenBSD: debug.h,v 1.1 2011/04/16 10:52:22 ratchov Exp $	*/
/*
 * Copyright (c) 2008 Alexandre Ratchov <alex@caoua.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#include <stdio.h>

#define DPRINTF(...)						\
	do {							\
		if (sndio_debug > 0)				\
			fprintf(stderr, __VA_ARGS__);		\
	} while(0)

#define DPERROR(s)						\
	do {							\
		if (sndio_debug > 0)				\
			perror(s);				\
	} while(0)

void sndio_debug_init(void);
extern int sndio_debug;
#else
#define DPRINTF(...) do {} while(0)
#define DPERROR(s) do {} while(0)
#endif

#endif
