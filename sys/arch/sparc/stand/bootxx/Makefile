#	$OpenBSD: Makefile,v 1.9 2007/11/25 18:25:33 deraadt Exp $
#	$NetBSD: Makefile,v 1.2 1995/09/30 21:43:38 pk Exp $

.PATH:	${.CURDIR}/../common

PROG=	bootxx

NOMAN=	noman
INSTALL_STRIP=
S=	${.CURDIR}/../../../..

SRCS=	srt0.S bootxx.c closeall.c dvma.c promdev.c
CLEANFILES+=${PROG}.aout ${PROG}.elf elfclean

.PATH:	${S}/lib/libkern/arch/sparc ${S}/lib/libkern
SRCS+=	__main.o bzero.o urem.o udiv.o

# pre-built bits of libsa
SOBJS=	alloc.o exit.o printf.o memcpy.o memset.o
_SOBJS=${SOBJS:S,^,${LIBSAOBJDIR}/,g}

CFLAGS= -O2 -I${.CURDIR}/../common -I${.CURDIR}/../../../../arch \
	-I${.CURDIR}/../../../.. -I${.CURDIR}/../../../../lib/libsa -DBOOTXX \
	${DEFS}

elfclean: clean-elf.c
	${HOSTCC} -o elfclean ${.ALLSRC}

${PROG}: ${OBJS} elfclean
	${LD} -N -Ttext ${RELOC} -e start ${OBJS} ${_SOBJS} -o ${.TARGET}.elf
	./elfclean ${.TARGET}.elf
	objcopy -j .text -j .data -j .bss -O a.out-sparc-netbsd \
	    ${.TARGET}.elf ${.TARGET}.aout
	@size ${.TARGET}.aout
	(echo -n 01 | tr 01 '\01\03'; tail +3c ${.TARGET}.aout) > ${.TARGET}
	@rm ${.TARGET}.aout ${.TARGET}.elf

srt0.o: srt0.S
	${CC} ${CFLAGS} -D_LOCORE -c ${.IMPSRC}

.include <bsd.prog.mk>
