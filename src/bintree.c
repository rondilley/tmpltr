/*****
 *
 * Copyright (c) 2011-2014, Ron Dilley
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
 *****/

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
