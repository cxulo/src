#	$OpenBSD: makeconf.awk,v 1.12 2003/05/18 07:18:27 mho Exp $
#	$NetBSD: makeconf.awk,v 1.3 1996/05/04 15:45:32 pk Exp $

#
# generate crunchgen(1) configuration file from `list' spec.
#

BEGIN {
	printf("#\n# This file is automatically generated by `makeconf'\n#\n\n");
	libs = "libs -lutil -lotermcap -ll -lm -ldes";
}

$1 == "LIBS" {
	$1 = tolower($1);
	libs = $0;
}

$1 == "SRCDIRS" {
	$1 = tolower($1);
	print;
}

($1 == "LINK" || $1 == "SYMLINK") && index($2,CBIN) {
	# find basenames for inclusion in crunchgen's `prog' and `ln' directives
	n = split($3, x, "/");
	p = x[n];
	progs[p] = NF - 3;
	for (i = 4; i <= NF; i++) {
		n = split($i, x, "/");
		l = x[n];
		links[i - 3, p] = l;
	}
}

$1 == "ARGVLINK" {
	# add extra `ln' entries (these don't appear in the filesystem)
	n = progs[$2];
	progs[$2] = ++n;
	links[n, $2] = $3;
}

$1 == "CRUNCHSPECIAL" {
	# collect crunchgen `special' directives
	$1 = "";
	specials[$0] = 1;
}

END {
	# write crunchgen configuration

	# `prog' directives; print 8 to a line
	column = 0;
	for (p in progs) {
		if ((column++ % 8) == 0)
			printf("\nprogs");
		printf(" %s", p);
	}
	printf("\n\n");

	# `ln' directives
	for (p in progs) {
		n = progs[p];
		for (i = 1; i <= n; i++)
			printf("ln %s %s\n", p, links[i,p]);
	}
	printf("\n%s\n\n", libs);

	# `special' directives
	for (s in specials) {
		printf("special %s\n", s);
	}
}
