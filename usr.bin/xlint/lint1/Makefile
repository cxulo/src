#	$OpenBSD: Makefile,v 1.10 2006/11/03 16:27:52 otto Exp $
#	$NetBSD: Makefile,v 1.3 1995/07/04 01:53:05 cgd Exp $

PROG=	lint1
SRCS=	cgram.y scan.l mem1.c mem.c err.c main1.c decl.c tree.c func.c \
	init.c emit.c emit1.c
NOMAN=
LDADD+=	-ll
DPADD+=	${LIBL}
YFLAGS=	-d
CFLAGS+=-I. -I${.CURDIR}
LINTFLAGS=-aehpz

BINDIR=	/usr/libexec

.include <bsd.prog.mk>
