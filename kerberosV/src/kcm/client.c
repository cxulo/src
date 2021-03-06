/*
 * Copyright (c) 2005, PADL Software Pty Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of PADL Software nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PADL SOFTWARE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL PADL SOFTWARE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "kcm_locl.h"

RCSID("$KTH: client.c,v 1.3 2005/02/06 01:22:48 lukeh Exp $");

krb5_error_code
kcm_ccache_resolve_client(krb5_context context,
			  kcm_client *client,
			  kcm_operation opcode,
			  const char *name,
			  kcm_ccache *ccache)
{
    krb5_error_code ret;

    ret = kcm_ccache_resolve(context, name, ccache);
    if (ret) {
	kcm_log(1, "Failed to resolve cache %s: %s",
		name, krb5_get_err_text(context, ret));
	return ret;
    }

    ret = kcm_access(context, client, opcode, *ccache);
    if (ret) {
	ret = KRB5_FCC_NOFILE; /* don't disclose */
	kcm_release_ccache(context, ccache);
    }

    return ret;
}

krb5_error_code
kcm_ccache_destroy_client(krb5_context context,
			  kcm_client *client,
			  const char *name)
{
    krb5_error_code ret;
    kcm_ccache ccache;

    ret = kcm_ccache_resolve(context, name, &ccache);
    if (ret) {
	kcm_log(1, "Failed to resolve cache %s: %s",
		name, krb5_get_err_text(context, ret));
	return ret;
    }

    ret = kcm_access(context, client, KCM_OP_DESTROY, ccache);
    if (ret) {
	kcm_release_ccache(context, &ccache);
	return ret;
    }

    ret = kcm_ccache_destroy(context, ccache->name);
    if (ret == 0) {
	/* don't leave any events dangling */
	kcm_cleanup_events(context, ccache);
    }

    kcm_release_ccache(context, &ccache);
    return ret;
}

krb5_error_code
kcm_ccache_new_client(krb5_context context,
		      kcm_client *client,
		      const char *name,
		      kcm_ccache *ccache_p)
{
    krb5_error_code ret;
    kcm_ccache ccache;

    /* We insist the ccache name starts with UID or UID: */
    if (name_constraints != 0) {
	char prefix[64];
	size_t prefix_len;
	int bad = 1;

	snprintf(prefix, sizeof(prefix), "%d:", client->uid);
	prefix_len = strlen(prefix);

	if (strncmp(name, prefix, prefix_len) == 0)
	    bad = 0;
	else {
	    prefix[prefix_len - 1] = '\0';
	    if (strcmp(name, prefix) == 0)
		bad = 0;
	}

	if (bad)
	    return KRB5_CC_BADNAME;
    }
	
    ret = kcm_ccache_resolve(context, name, &ccache);
    if (ret == 0) {
	if (ccache->uid != client->uid ||
	    ccache->gid != client->gid)
	    return KRB5_FCC_PERM;
    } else if (ret != KRB5_FCC_NOFILE) {
	return ret;
    }

    if (ret == KRB5_FCC_NOFILE) {
	ret = kcm_ccache_new(context, name, &ccache);
	if (ret) {
	    kcm_log(1, "Failed to initialize cache %s: %s",
		    name, krb5_get_err_text(context, ret));
	    return ret;
	}

	/* bind to current client */
	ccache->uid = client->uid;
	ccache->gid = client->gid;
    } else {
	ret = kcm_zero_ccache_data(context, ccache);
	if (ret) {
	    kcm_log(1, "Failed to empty cache %s: %s",
		    name, krb5_get_err_text(context, ret));
	    kcm_release_ccache(context, &ccache);
	    return ret;
	}
	kcm_cleanup_events(context, ccache);
    }

    ret = kcm_access(context, client, KCM_OP_INITIALIZE, ccache);
    if (ret) {
	kcm_release_ccache(context, &ccache);
	kcm_ccache_destroy(context, name);
	return ret;
    }

    *ccache_p = ccache;
    return 0;
}

