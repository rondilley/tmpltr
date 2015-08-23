/*****
 *
 * Description: Template Matching Headers
 * 
 * Copyright (c) 2008-2015, Ron Dilley
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

#ifndef MATCH_DOT_H
#define MATCH_DOT_H

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
#include "util.h"
#include "mem.h"
#include "hash.h"
#include "parser.h"

/****
 *
 * defines
 *
 ****/


/****
 *
 * typdefs & structs
 *
 ****/

struct templateMatchList_s {
  char *template;
  int len;
  int count;
  struct templateMatchList_s *next;
};

/****
 *
 * function prototypes
 *
 ****/

int addMatchTemplate( char *template );
int loadMatchTemplates( char *fName );
int addMatchLine( char *line );
int loadMatchLines( char *fName );
int templateMatches( char *template );
void cleanMatchList( void );

#endif /* end of MATCH_DOT_H */

