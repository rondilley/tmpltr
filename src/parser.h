/*****
 *
 * Description: Line Parser Headers
 * 
 * Copyright (c) 2008-2021, Ron Dilley
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <config.h>
#endif

#include <sysdep.h>

#ifndef __SYSDEP_H__
#error something is messed up
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
#define FIELD_TYPE_DT_SYSLOG 40 /* mmm dd hh:mm:ss */
#define FIELD_TYPE_DT 41        /* mm/dd/yyyy hh:mm:ss */
#define FIELD_TYPE_DT_FULL 42   /* mm/dd/yyyy hh:mm:ss.s */

#define PARSER_MIN_FIELD_LEN 0

#define FIELD_TYPE_STRING_TOK 's'
#define FIELD_TYPE_INT_TOK 'd'
#define FIELD_TYPE_FLOAT_TOK 'f'
#define FIELD_TYPE_HEX_TOK 'x'
#define FIELD_TYPE_CHAR_TOK 'c'
#define FIELD_TYPE_IP4_TOK 'i'
#define FIELD_TYPE_IP6_TOK 'I'
#define FIELD_TYPE MACADDR_TOK 'm'
#define FIELD_TYPE_SYSLOGDT_TOK 'D'
#define FIELD_TYPE_DT_TOK 't'
#define FIELD_TYPE_FULLDT_TOK 'T'

#define MAX_FIELD_POS 2048
#define MAX_FIELD_LEN 16384

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

void initParser(void);
void deInitParser(void);
int parseLine(char *line);
int getParsedField(char *oBuf, int oBufLen, const unsigned int fieldNum);
void showCounts( void );

#endif /* end of PARSER_DOT_H */
