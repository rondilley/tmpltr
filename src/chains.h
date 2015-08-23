/*****
 *
 * Description: Log Chain Headers
 * 
 * Copyright (c) 2011-2015, Ron Dilley
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

#ifndef CHAINS_DOT_H
#define CHAINS_DOT_H

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

struct chainEntry_s {
  struct hash_s *fields;
  int fieldCount;
  char **templates;
  int templateCount;
  time_t lastUpdated;
  struct chainEntry_s *prev;
  struct chainEntry_s *next;
};

typedef struct {
  struct chainEntry_s *head;
  struct chainEntry_s *tail;
  int chainCount;  
} chainList_t;


/****
 *
 * function prototypes
 *
 ****/

#endif /* end of CHAINS_DOT_H */

