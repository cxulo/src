/*
 * Copyright (c) 1994 Mats O Jansson <moj@stacken.kth.se>
 * All rights reserved.
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
 *
 *	$Id: ypdef.h,v 1.2 1994/12/17 22:27:26 moj Exp $
 */

#ifndef _YPDEF_H_
#define _YPDEF_H_

#define YP_DB_PATH "/var/yp"
#define YP_LAST_KEY        "YP_LAST_MODIFIED"
#define YP_LAST_LEN	   sizeof(YP_LAST_KEY)
#define YP_INPUT_KEY       "YP_INPUT_FILE"
#define YP_INPUT_LEN	   sizeof(YP_INPUT_KEY)
#define YP_OUTPUT_KEY      "YP_OUTPUT_FILE"
#define YP_OUTPUT_LEN	   sizeof(YP_OUTPUT_KEY)
#define YP_MASTER_KEY      "YP_MASTER_NAME"
#define YP_MASTER_LEN	   sizeof(YP_MASTER_KEY)
#define YP_DOMAIN_KEY      "YP_DOMAIN_NAME"
#define YP_DOMAIN_LEN	   sizeof(YP_DOMAIN_KEY)
#define YP_INTERDOMAIN_KEY "YP_INTERDOMAIN"
#define YP_INTERDOMAIN_LEN sizeof(YP_INTERDOMAIN_KEY)
#define YP_SECURE_KEY      "YP_SECURE"
#define YP_SECURE_LEN      sizeof(YP_SECURE_KEY)

#define MAX_LAST_LEN 10
#define MAX_MASTER_LEN 255
#define YP_HOSTNAME "hosts.byname"
#define YP_HOSTADDR "hosts.byaddr"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define YPXFR_PROC "/usr/sbin/ypxfr"

#endif /* !_YPDEF_H_ */
