/*****
 *
 * Description: Main Headers
 * 
 * Copyright (c) 2008-2025, Ron Dilley
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

#ifndef MAIN_DOT_H
#define MAIN_DOT_H

/****
 *
 * defines
 *
 ****/

#define PROGNAME "tmpltr"
#define MAX_ARGS_IN_FIELD 2
#define ALARM_TIMER 1

/****
 *
 * includes
 *
 ****/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
# error something is messed up
#endif

#include "../include/common.h"
#include "util.h"
#include "mem.h"
#include "tmpltr.h"
#include "match.h"
#include "string_intern.h"

/****
 *
 * consts & enums
 *
 ****/

/****
 *
 * typedefs & structs
 *
 ****/

/****
 *
 * function prototypes
 *
 ****/

int main(int argc, char *argv[]);
PRIVATE void print_version( void );
PRIVATE void print_help( void );
PRIVATE void cleanup( void );
void ctime_prog( int signo );

#endif /* MAIN_DOT_H */

