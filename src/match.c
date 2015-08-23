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

#include "match.h"

/****
 * 
 * local variables
 *
 ****/

struct templateMatchList_s *matchTemplates = NULL;

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
 * add template to match list
 *
 ****/

int addMatchTemplate( char *template ) {
  int templateLen = strlen( template );
  struct templateMatchList_s *head = matchTemplates;
  struct templateMatchList_s *tmpMatch = XMALLOC( sizeof( struct templateMatchList_s ) );

  fprintf( stderr, "Adding template to search list [%s]\n", template );

  XMEMSET( tmpMatch, 0, sizeof( struct templateMatchList_s ) );
  if ( templateLen > MAX_FIELD_LEN ) {
    fprintf( stderr, "ERR - Match template too long\n" );
    XFREE( tmpMatch );
    return FALSE;
  }
  tmpMatch->template = XMALLOC( templateLen + 1 );
  XMEMSET( tmpMatch->template, 0, templateLen + 1 );
  XMEMCPY( tmpMatch->template, template, templateLen );
  tmpMatch->len = templateLen;

  if ( head EQ NULL )
    head = tmpMatch;
  else {
    while( head->next != NULL )
      head = head->next;
    head->next = tmpMatch;
  }
  return TRUE;
}

/****
 *
 * load match templates from file
 *
 ****/

int loadMatchTemplates( char *fName ) {
  FILE *inFile;
  char inBuf[8192];
  size_t count = 0;
  int lLen, i;

#ifdef DEBUG
  if ( config->debug >= 1 )
    printf( "DEBUG - Loading match template file [%s]\n", fName );
#endif

  if ( ( inFile = fopen( fName, "r" ) ) EQ NULL ) {
    fprintf( stderr, "ERR - Unable to open match template file [%s]\n", fName );
    return( FAILED );
  }

  while( fgets( inBuf, sizeof( inBuf ), inFile ) != NULL ) {
    if ( inBuf[0] != '#' ) {
      /* strip of <CR> or <LF> */
      lLen = strlen( inBuf );
      for( i = 0; i < lLen; i++ ) {
	if ( inBuf[i] EQ '\n' | inBuf[i] EQ '\r' ) {
	  inBuf[i] = '\0';
	  i = lLen;
	}
      }

#ifdef DEBUG
      if ( config->debug >= 3 )
	printf( "DEBUG - Loading match template [%s]\n", inBuf );
#endif

      count++;
      addMatchTemplate( inBuf );
    }
  }

  fclose( inFile );

#ifdef DEBUG
  if ( config->debug >= 6 ) {
	  struct templateMatchList_s *matchPtr = matchTemplates;

	printf( "DEBUG - matchPtr: 0x%08x", matchPtr );
	
	  /* search templates for match */
	  while( matchPtr != NULL ) {
	      printf( "DEBUG - Loaded [%s]\n", matchPtr->template );
	    matchPtr = matchPtr->next;
	  }
	}
	
  if ( config->debug >= 1 )
    printf( "DEBUG - Loaded [%lu] match templates\n", count );
#endif

  return( EXIT_SUCCESS );
}

/****
 *
 * convert log line to template and add to match list
 *
 ****/

int addMatchLine( char *line ) {
  char oBuf[4096];

  if ( parseLine( line ) > 0 ) {
    getParsedField( oBuf, sizeof( oBuf ), 0 );
    addMatchTemplate( oBuf );
    return TRUE;
  }

  return FALSE;
}

/****
 *
 * load match lines from file and convert to templates
 *
 ****/

int loadMatchLines( char *fName ) {
  FILE *inFile;
  char inBuf[8192];
  size_t count = 0;
  int lLen, i;

#ifdef DEBUG
  if ( config->debug >= 1 )
    printf( "DEBUG - Loading match line file [%s]\n", fName );
#endif

  if ( ( inFile = fopen( fName, "r" ) ) EQ NULL ) {
    fprintf( stderr, "ERR - Unable to open match line file [%s]\n", fName );
    return( FAILED );
  }

  while( fgets( inBuf, sizeof( inBuf ), inFile ) != NULL ) {
    if ( inBuf[0] != '#' ) {
      /* strip of <CR> */
      lLen = strlen( inBuf );
      for( i = 0; i < lLen; i++ ) {
	if ( inBuf[i] EQ '\n' ) {
	  inBuf[i] = '\0';
	  i = lLen;
	}
      }

      
#ifdef DEBUG
      if ( config->debug >= 3 )
	printf( "DEBUG - Loading match line [%s]\n", inBuf );
#endif

      count++;
      addMatchLine( inBuf );
    }
  }

  fclose( inFile );

#ifdef DEBUG
  if ( config->debug >= 1 )
    printf( "DEBUG - Loaded [%lu] match lines\n", count );
#endif

  return( TRUE );
}

/****
 *
 * test for match
 *
 ****/

int templateMatches( char *template ) {
  int i, match = TRUE;
  int templateLen = strlen( template );
  struct templateMatchList_s *matchPtr = matchTemplates;

  /* search templates for match */
  while( matchPtr != NULL ) {
#ifdef DEBUG
    if ( config->debug >= 3 )
      printf( "DEBUG - Compairing [%s] to [%s]\n", template, matchPtr->template );
#endif
    if ( matchPtr->len EQ templateLen ) {
      for( i = templateLen; match EQ TRUE && i >= 0; i-- )
	if ( template[i] != matchPtr->template[i] )
	  match = FALSE;
      if ( match EQ TRUE )
	return TRUE;
    }
    matchPtr = matchPtr->next;
    match = TRUE;
  }

  /* if template matches, increment match count */

  return FALSE;
}

/****
 *
 * cleanup match list
 *
 ****/

void cleanMatchList( void ) {
  struct templateMatchList_s *matchPtr;

  while( matchTemplates != NULL ) {
    matchPtr = matchTemplates;
    if ( matchPtr->template != NULL )
      XFREE( matchPtr->template );
    matchTemplates = matchTemplates->next;
    XFREE( matchPtr );
  }
}
