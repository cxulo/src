#	$OpenBSD: Makefile,v 1.9 2011/01/26 15:10:59 espie Exp $

MAN=makewhatis.8
NOPROG=
PACKAGES= \
	OpenBSD/Makewhatis.pm \
	OpenBSD/Makewhatis/Check.pm \
	OpenBSD/Makewhatis/Find.pm \
	OpenBSD/Makewhatis/Formated.pm \
	OpenBSD/Makewhatis/Subject.pm \
	OpenBSD/Makewhatis/Unformated.pm \
	OpenBSD/Makewhatis/Whatis.pm

SCRIPTS= \
	makewhatis

LIBBASE=/usr/libdata/perl5

realinstall: 
.for i in ${PACKAGES}
	${INSTALL} -d -o ${LIBOWN} -g ${LIBGRP} -m ${DIRMODE} \
		${DESTDIR}${LIBBASE}/${i:H}
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
		${.CURDIR}/$i ${DESTDIR}${LIBBASE}/$i
.endfor
.for i in ${SCRIPTS}
	${INSTALL} ${INSTALL_COPY} -o ${BINOWN} -g ${BINGRP} -m ${BINMODE} \
		${.CURDIR}/$i ${DESTDIR}${BINDIR}/$i
.endfor

.include <bsd.prog.mk>
