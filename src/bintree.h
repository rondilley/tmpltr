/*****
 *
 * Description: Binary Tree Headers
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

#ifndef BINTREE_DOT_H
#define BINTREE_DOT_H

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
#include "mem.h"

/****
 *
 * typedefs and enums
 *
 ****/

struct binTree_s {
  char *value;
  struct binTree_s *left;
  struct binTree_s *right;
};

/****
 *
 * function prototypes
 *
 ****/

void destroyBinTree( struct binTree_s *node );
void insertBinTree( struct binTree_s **node, char *value );
struct binTree_s *searchBinTree( struct binTree_s *node, char *value );

#endif /* end of BINTREE_DOT_H */

