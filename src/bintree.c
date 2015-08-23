/*****
 *
 * Description: Binary Tree Functions
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

/****
 *
 * defines
 *
 ****/

/****
 *
 * includes
 *
 ****/

#include "bintree.h"

/****
 *
 * local variables
 *
 ****/

/****
 *
 * external global variables
 *
 ****/

extern Config_t *config;

/****
 *
 * functions
 *
 ****/

/****
 *
 * empty binary tree
 *
 ****/

inline void destroyBinTree( struct binTree_s *node ) {
  if( node != 0 ) {
    destroyBinTree( node->left );
    destroyBinTree( node->right );
    XFREE( node->value );
    XFREE( node );
  }
}

/****
 *
 * insert record into binary tree
 *
 ****/

inline void insertBinTree( struct binTree_s **node, char *value ) {
  if ( *node EQ 0 ) {
    *node = (struct binTree_s*)XMALLOC( sizeof( struct binTree_s ) );
    (*node)->value = (char *)XMALLOC( strlen( value ) + 1 );
    XSTRNCPY( (*node)->value, value, strlen( value ) );
    (*node)->left = NULL;    
    (*node)->right = NULL;  
  } else if ( strcmp( value, (*node)->value ) < 0 )
    insertBinTree( &(*node)->left, value );
  else if ( strcmp( value, (*node)->value ) > 0 )
    insertBinTree( &(*node)->right, value );
}

/****
 *
 * search for value in binary tree
 *
 ****/

inline struct binTree_s *searchBinTree( struct binTree_s *node, char *value ) {
  if( node != 0 ) {
    if( strcmp( value, node->value ) EQ 0 )
      return node;
    else if (strcmp( value, node->value ) < 0 )
      return searchBinTree( node->left, value );
    else
      return searchBinTree( node->right, value );
  } else
    return 0;
}
