/****
 *
 * Copyright (c) 2014, Ron Dilley
 * All rights reserved.
 *
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of Uberadmin/BaraCUDA/Nightingale nor the names of
*     its contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****/

#ifndef PARSER_DOT_H
#define PARSER_DOT_H

/****
 *
 * includes
 *
 ****/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sysdep.h>

#ifndef __SYSDEP_H__
# error something is messed up
#endif

#include <common.h>
#include "chains.h"
#include "util.h"
#include "mem.h"

/****
 *
 * defines
 *
 ****/

/* flow control */
#define FIELD_TYPE_UNDEF 0
#define FIELD_TYPE_EXTRACT 1
#define FIELD_TYPE_STATIC 2

/* alpha */
#define FIELD_TYPE_STRING 10
#define FIELD_TYPE_CHAR 11

/* numbers */
#define FIELD_TYPE_NUM_INT 20
#define FIELD_TYPE_NUM_FLOAT 21
#define FIELD_TYPE_NUM_HEX 22

/* addresses */
#define FIELD_TYPE_IP4 30
#define FIELD_TYPE_IP6 31
#define FIELD_TYPE_MACADDR 32

/* dates */
#define FIELD_TYPE_DT_SYSLOG 40

#define PARSER_MIN_FIELD_LEN 0

#define MAX_FIELD_POS 1024
#define MAX_FIELD_LEN 2048

/****
 *
 * typdefs & structs
 *
 ****/

/****
 *
 * function prototypes
 *
 ****/

void initParser( void );
void deInitParser( void );
int parseLine( char *line );
int getParsedField( char *oBuf, int oBufLen, const unsigned int fieldNum );

#endif /* end of PARSER_DOT_H */

