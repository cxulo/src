/*	$OpenBSD: certpatch.c,v 1.21 2003/06/04 07:31:17 ho Exp $	*/
/*	$EOM: certpatch.c,v 1.11 2000/12/21 14:50:09 ho Exp $	*/

/*
 * Copyright (c) 1999 Niels Provos.  All rights reserved.
 * Copyright (c) 1999, 2000 Angelos D. Keromytis.  All rights reserved.
 * Copyright (c) 2000, 2001 Niklas Hallqvist.  All rights reserved.
 * Copyright (c) 2001 H�kan Olsson.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This code was written under funding by Ericsson Radio Systems.
 */

/*
 * This program takes a certificate generated by ssleay and a
 * private key. It encodes a new id as subject alt name
 * extension into the certifcate. The result gets written as
 * new certificate that can be used by isakmpd.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sysdep.h"

#ifdef KAME
#  ifdef CRYPTO
#    include <openssl/rsa.h>
#  endif
#else
#  include <openssl/rsa.h>
#endif

#include <openssl/x509.h>
#include <openssl/pem.h>

#include "conf.h"
#include "ipsec_num.h"
#include "log.h"
#include "math_mp.h"
#include "x509.h"

#define IDTYPE_IP	"ip"
#define IDTYPE_FQDN     "fqdn"
#define IDTYPE_UFQDN    "ufqdn"

int
main (int argc, char **argv)
{
  char *usage = "%s [-t idtype] -i id -k keyfile certin certout\n\n"
    "This programs takes a certificate and adds a subjectAltName extension\n"
    "with the identication given as command line argument.  Be sure that \n"
    "the signing key matches the issuer.\n";
  EVP_PKEY *pkey_priv;
  X509 *cert;
  BIO *file;
  const EVP_MD *digest;
  X509_EXTENSION *ex = NULL;
  ASN1_OCTET_STRING *data = NULL;
  struct in_addr saddr;
  unsigned char ipaddr[6], *new_id;
  char *type = IDTYPE_IP, *keyfile = NULL, *id = NULL;
  char *certin, *certout;
  int ch, err;

#if SSLEAY_VERSION_NUMBER >= 0x00904100L
  unsigned char *p;
  ASN1_STRING str;
  int i;
#endif


  /* read command line arguments */
  while ((ch = getopt (argc, argv, "t:k:i:")) != -1)
    switch (ch) {
    case 't':
      type = optarg;
      break;
    case 'k':
      keyfile = optarg;
      break;
    case 'i':
      id = optarg;
      break;
    default:
      fprintf (stderr, usage, argv[0]);
      return (1);
    }

  argc -= optind;

  if (argc != 2) {
    fprintf (stderr, usage, argv[0]);
    return (1);
  }

  argv += optind;

  certin = argv[0];
  certout = argv[1];

  /* Check ID */

  if ((strcasecmp (IDTYPE_IP, type) != 0 &&
       strcasecmp (IDTYPE_FQDN, type) != 0 &&
       strcasecmp (IDTYPE_UFQDN, type) != 0) || id == NULL)
    {
      printf ("wrong id type or missing id\n");
      return (1);
    }

  /*
   * X509_verify will fail, as will all other functions that call
   * EVP_get_digest_byname.
   */

  SSLeay_add_all_algorithms ();

  /* Use a certificate created by ssleay and add the appr. extension */
  printf ("Reading ssleay created certificate %s and modify it\n",
	  certin);
  file = BIO_new (BIO_s_file ());
  if (BIO_read_filename (file, certin) == -1)
    {
      perror ("read");
      return (1);
    }
#if SSLEAY_VERSION_NUMBER >= 0x00904100L
  cert = PEM_read_bio_X509 (file, NULL, NULL, NULL);
#else
  cert = PEM_read_bio_X509 (file, NULL, NULL);
#endif
  BIO_free (file);
  if (cert == NULL)
    {
      printf ("PEM_read_bio_X509 () failed\n");
      return (1);
    }

  /* Get the digest for the actual signing */
  digest = EVP_get_digestbyname (OBJ_nid2sn (OBJ_obj2nid (cert->sig_alg->algorithm)));

  if (!X509_set_version (cert, 2))
    {
      printf ("X509 failed to set version number\n");
      return (1);
    }

  if (!strcasecmp (IDTYPE_IP, type))
    {
      if (inet_aton (id, &saddr) == 0)
        {
	  printf ("inet_aton () failed\n");
	  return (1);
	}

      saddr.s_addr = htonl (saddr.s_addr);
      ipaddr[0] = 0x87;
      ipaddr[1] = 0x04;
      ipaddr[2] = saddr.s_addr >> 24;
      ipaddr[3] = (saddr.s_addr >> 16) & 0xff;
      ipaddr[4] = (saddr.s_addr >> 8) & 0xff;
      ipaddr[5] = saddr.s_addr & 0xff;

#if SSLEAY_VERSION_NUMBER >= 0x00904100L
      str.length = 6;
      str.type = V_ASN1_OCTET_STRING;
      str.data = ipaddr;
      data = ASN1_OCTET_STRING_new ();
      if (!data)
	{
	  perror ("ASN1_OCTET_STRING_new() failed");
	  return (1);
	}

      i = i2d_ASN1_OCTET_STRING ((ASN1_OCTET_STRING *)&str, NULL);
      if (!ASN1_STRING_set ((ASN1_STRING *)data,NULL,i))
        {
	  perror ("ASN1_STRING_set() failed");
	  return (1);
	}
      p = (unsigned char *)data->data;
      i2d_ASN1_OCTET_STRING ((ASN1_OCTET_STRING *)&str, &p);
      data->length = i;
#else
      data = X509v3_pack_string (NULL, V_ASN1_OCTET_STRING, ipaddr, 6);
#endif
    }
  else if (!strcasecmp (IDTYPE_FQDN, type) || !strcasecmp (IDTYPE_UFQDN, type))
    {
      new_id = malloc (strlen (id) + 2);
      if (new_id == NULL)
        {
          printf ("malloc () failed\n");
          return (1);
        }

      if (!strcasecmp (IDTYPE_FQDN, type))
        new_id[0] = 0x82;
      else
        new_id[0] = 0x81; /* IDTYPE_UFQDN */

      memcpy (new_id + 2, id, strlen(id));
      new_id[1] = strlen (id);
#if SSLEAY_VERSION_NUMBER >= 0x00904100L
      str.length = strlen (id) + 2;
      str.type = V_ASN1_OCTET_STRING;
      str.data = new_id;
      data = ASN1_OCTET_STRING_new ();
      if (!data)
        {
          perror ("ASN1_OCTET_STRING_new() failed");
          return (1);
        }

      i = i2d_ASN1_OCTET_STRING ((ASN1_OCTET_STRING *)&str, NULL);
      if (!ASN1_STRING_set ((ASN1_STRING *)data,NULL,i))
        {
          perror ("ASN1_STRING_set() failed");
          return (1);
        }
      p = (unsigned char *)data->data;
      i2d_ASN1_OCTET_STRING ((ASN1_OCTET_STRING *)&str, &p);
      data->length = i;
#else
      data = X509v3_pack_string (NULL, V_ASN1_OCTET_STRING, new_id,
				 strlen (id) + 2);
#endif
      free (new_id);
    }

  /* XXX This is a hack, how to do better?  */
  data->type = 0x30;
  data->data[0] = 0x30;
  ex = X509_EXTENSION_create_by_NID (NULL, NID_subject_alt_name, 1, data);

  if (ex == NULL)
    {
      printf ("X509_EXTENSION_create ()\n");
      return (1);
    }

  X509_add_ext (cert, ex, -1);

  file = BIO_new (BIO_s_file ());
  if (BIO_read_filename (file, keyfile) == -1)
    {
      perror ("open");
      return (1);
    }
#if SSLEAY_VERSION_NUMBER >= 0x00904100L
  if ((pkey_priv = PEM_read_bio_PrivateKey (file, NULL, NULL, NULL)) == NULL)
#else
  if ((pkey_priv = PEM_read_bio_PrivateKey (file, NULL, NULL)) == NULL)
#endif
    {
      printf ("Can not read private key %s\n", keyfile);
      return (1);
    }
  BIO_free (file);

  printf ("Creating Signature: PKEY_TYPE = %s: ",
	  pkey_priv->type == EVP_PKEY_RSA ? "RSA" : "unknown");
  err = X509_sign (cert, pkey_priv, digest);
  printf ("X509_sign: %d ", err);
  if (!err)
    printf ("FAILED ");
  else
    printf ("OKAY ");
  printf ("\n");

  file = BIO_new (BIO_s_file ());
  if (BIO_write_filename (file, certout) == -1)
    {
      perror ("open");
      return (1);
    }

  printf ("Writing new certificate to %s\n", certout);
  PEM_write_bio_X509 (file, cert);
  BIO_free (file);

  return (0);
}
