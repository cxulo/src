/*	$OpenBSD: server.c,v 1.1.1.1 2004/07/13 22:02:40 jfb Exp $	*/
/*
 * Copyright (c) 2004 Jean-Francois Brousseau <jfb@openbsd.org>
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <paths.h>
#include <sysexits.h>

#include "cvs.h"
#include "log.h"
#include "sock.h"


extern struct cvsroot *cvs_root;



/* argument vector built by the `Argument' and `Argumentx' requests */
char   **cvs_args;
u_int    cvs_nbarg = 0;

u_int   cvs_utf8ok = 0;
u_int   cvs_case   = 0;



/*
 * cvs_server()
 *
 * Implement the `cvs server' command.  As opposed to the general method of
 * CVS client/server implementation, the cvs program merely acts as a
 * redirector to the cvs daemon for most of the tasks.
 *
 * The `cvs server' command is only used on the server side of a remote
 * cvs command.  With this command, the cvs program starts listening on
 * standard input for CVS protocol requests.
 */

int
cvs_server(int argc, char **argv)
{
	ssize_t ret;
	char reqbuf[128];

	if (argc != 1) {
		return (EX_USAGE);
	}

	for (;;) {
		ret = read(STDIN_FILENO, reqbuf, sizeof(reqbuf));
		if (ret == 0) {
			break;
		}


	}


	if (cvs_sock_connect(cvs_root->cr_dir) < 0) {
		cvs_log(LP_ERR, "failed to connect to CVS server socket");
		return (-1);
	}

	cvs_sock_disconnect();

	return (0);
}
