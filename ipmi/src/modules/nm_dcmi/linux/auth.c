/*
 * Copyright (c) 2003 Sun Microsystems, Inc.  All Rights Reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ipmitool_include/ipmitool_helper.h"
#include "ipmitool_include/ipmitool_bswap.h"
#include "ipmitool_include/ipmitool_ipmi.h"
#include "ipmitool_include/ipmitool_ipmi_intf.h"

#include "ipmitool_include/ipmitool_md5.h"

/*
 * multi-session authcode generation for MD5
 * H(password + session_id + msg + session_seq + password)
 *
 * Use OpenSSL implementation of MD5 algorithm if found
 */
uint8_t * ipmi_auth_md5(struct ipmi_session * s, uint8_t * data, int data_len)
{
	md5_state_t state;
	static md5_byte_t digest[16];
	uint32_t temp;

	memset(digest, 0, 16);
	memset(&state, 0, sizeof(md5_state_t));

	md5_init(&state);

	md5_append(&state, (const md5_byte_t *)s->authcode, 16);
	md5_append(&state, (const md5_byte_t *)&s->session_id, 4);
	md5_append(&state, (const md5_byte_t *)data, data_len);

        if(!isLittleEndian())
	 temp = BSWAP_32(s->in_seq);
        else
	 temp = s->in_seq;

	md5_append(&state, (const md5_byte_t *)&temp, 4);
	md5_append(&state, (const md5_byte_t *)s->authcode, 16);

	md5_finish(&state, digest);

	if (verbose > 3)
		printf("  MD5 AuthCode    : %s\n", buf2str(digest, 16));
	return digest;
}

/* 
 * multi-session authcode generation for MD2
 * H(password + session_id + msg + session_seq + password)
 *
 * Use OpenSSL implementation of MD2 algorithm if found.
 * This function is analogous to ipmi_auth_md5
 */
uint8_t * ipmi_auth_md2(struct ipmi_session * s, uint8_t * data, int data_len)
{
	static uint8_t md[16];
	memset(md, 0, 16);
	printf("WARNING: No internal support for MD2!  "
	       "Please re-compile with OpenSSL.\n");
	return md;
}

/* special authentication method */
uint8_t * ipmi_auth_special(struct ipmi_session * s)
{
	int i;
	md5_state_t state;
	static md5_byte_t digest[16];
	uint8_t challenge[16];

	memset(challenge, 0, 16);
	memset(digest, 0, 16);
	memset(&state, 0, sizeof(md5_state_t));

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)s->authcode, strlen((const char*)s->authcode));
	md5_finish(&state, digest);

	for (i=0; i<16; i++)
		challenge[i] = s->challenge[i] ^ digest[i];

	memset(digest, 0, 16);
	memset(&state, 0, sizeof(md5_state_t));

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)challenge, 16);
	md5_finish(&state, digest);

	return digest;
}

