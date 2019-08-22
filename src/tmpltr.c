/*****
 *
 * Description: Log Templater Functions
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

/****
 *
 * includes
 *
 ****/

#include "tmpltr.h"

/****
 *
 * local variables
 *
 ****/

/****
 *
 * global variables
 *
 ****/

/* hashes */
struct hash_s *templateHash = NULL;

/****
 *
 * external variables
 *
 ****/

extern int errno;
extern char **environ;
extern Config_t *config;
extern int quit;
extern int reload;

/****
 *
 * functions
 *
 ****/

/****
 *
 * assemble clustered template string
 *
 ****/

char *clusterTemplate( char *template, metaData_t *md, char *oBuf, int bufSize ) {
  int i, rPos = 0, wPos = 0, done;
  struct Fields_s *curFieldPtr = md->head;
  
  /* loop through the field argument trees */
  while( ( curFieldPtr != NULL ) ) {
    /* find next variable */

    /* XXX make it a state machine */
    done = FALSE;
    while( ! done ) {
      while( ( template[rPos] != 0 ) & ( template[rPos] != '%' ) & ( wPos < bufSize ) )
	oBuf[wPos++] = template[rPos++];

      if ( ( wPos >= bufSize ) | ( template[rPos] EQ 0 ) ) {
	oBuf[wPos] = 0;
	done = TRUE;
      } else if ( template[rPos] EQ '%' ) {
	if ( ( template[rPos+1] EQ 'd' ) |
	     ( template[rPos+1] EQ 's' ) |
	     ( template[rPos+1] EQ 'c' ) |
	     ( template[rPos+1] EQ 'x' ) |
	     ( template[rPos+1] EQ 'm' ) |	  
	     ( template[rPos+1] EQ 'i' ) |
	     ( template[rPos+1] EQ 'I' ) ) {
	  if ( curFieldPtr->count EQ 1 ) {
	    /* move past the place holder */
	    rPos += 2;
	    for( i = 1; curFieldPtr->head->value[i] != 0; i++ )
	      oBuf[wPos++] = curFieldPtr->head->value[i];
	  } else {
	    oBuf[wPos++] = template[rPos++];
	    oBuf[wPos++] = template[rPos++];
	  }
	  done = TRUE;
	} else
	  oBuf[wPos++] = template[rPos++];
      } else
	oBuf[wPos++] = template[rPos++];
    }
    curFieldPtr = curFieldPtr->next;
  }

  /* copy the rest of the chatacters */
  while( ( template[rPos] != 0 ) & ( wPos < bufSize ) )
    oBuf[wPos++] = template[rPos++];
  oBuf[wPos] = 0;

  return( oBuf );
}

/****
 *
 * print all template records in hash
 *
 ****/

int printTemplate( const struct hashRec_s *hashRec ) {
  metaData_t *tmpMd;
  struct Fields_s *curFieldPtr, *tmpFieldPtr;
  char oBuf[4096];

#ifdef DEBUG
  if ( config->debug >= 3 )
    printf( "DEBUG - Searching for [%s]\n", hashRec->keyString );
#endif
  
  if ( hashRec->data != NULL ) {
    tmpMd = (metaData_t *)hashRec->data;
    /* save template if -w was used */
    if ( config->outFile_st != NULL )
      fprintf( config->outFile_st, "%s\n", hashRec->keyString );
    
    if ( config->cluster ) {
      if ( tmpMd->count > 1 )
	printf( "%12lu %s||%s", tmpMd->count, clusterTemplate( hashRec->keyString, tmpMd, oBuf, sizeof( oBuf ) ), tmpMd->lBuf );
      else
	printf( "%12lu %s\n", tmpMd->count, clusterTemplate( hashRec->keyString, tmpMd, oBuf, sizeof( oBuf ) ) );
    } else
      printf( "%12lu %s||%s", tmpMd->count, hashRec->keyString, tmpMd->lBuf );

    /* cleanup after clustering */
    if ( config->cluster ) {
      if ( tmpMd != NULL )
        curFieldPtr = tmpMd->head;
      else
        curFieldPtr = NULL;
    
      while( curFieldPtr != NULL ) {
        tmpFieldPtr = curFieldPtr;
        curFieldPtr = curFieldPtr->next;
        destroyBinTree( tmpFieldPtr->head );
        XFREE( tmpFieldPtr );
      }
    }
  }
  
  /* can use this later to interrupt traversing the hash */
  if ( quit )
    return( TRUE );
  return( FALSE );
}

/****
 *
 * process file
 *
 ****/

int processFile( const char *fName ) {
  FILE *inFile = NULL, *outFile = NULL;
  char inBuf[8192];
  char outFileName[PATH_MAX];
  char patternBuf[4096];
  char oBuf[4096];
  PRIVATE int c = 0, i, ret;
  unsigned int lineCount = 0, lineLen = 0, minLineLen = sizeof(inBuf), maxLineLen = 0, totLineLen = 0;
  unsigned int argCount = 0, totArgCount = 0, minArgCount = MAX_FIELD_POS, maxArgCount = 0;
  struct hashRec_s *tmpRec;
  metaData_t *tmpMd;
  struct Fields_s **curFieldPtr;

  /* initialize the hash if we need to */
  if ( templateHash EQ NULL )
    templateHash = initHash( 52 );

  initParser();

  fprintf( stderr, "Opening [%s] for read\n", fName );
  if ( strcmp( fName, "-" ) EQ 0 ) {
    inFile = stdin;
  } else {
#ifdef HAVE_FOPEN64
    if ( ( inFile = fopen64( fName, "r" ) ) EQ NULL ) {
#else
    if ( ( inFile = fopen( fName, "r" ) ) EQ NULL ) {
#endif
      fprintf( stderr, "ERR - Unable to open file [%s] %d (%s)\n", fName, errno, strerror( errno ) );
      return( EXIT_FAILURE );
    }
  }

  while( fgets( inBuf, sizeof( inBuf ), inFile ) != NULL && ! quit ) {
    if ( reload EQ TRUE ) {
      fprintf( stderr, "Processed %d lines/min\n", lineCount );
#ifdef DEBUG
      if ( config->debug ) {
	fprintf( stderr, "Line length: min=%d, max=%d, avg=%2.0f\n", minLineLen, maxLineLen, (float)totLineLen / (float)lineCount );

	if ( config->cluster ) {
	  fprintf( stderr, "Stored %d args/min\n", argCount );
	  fprintf( stderr, "Arg count: min/%d, max/%d, avg/%2.0f\n", minArgCount, maxArgCount, (float)totArgCount / (float)lineCount );
	  minArgCount = MAX_FIELD_POS;
	  maxArgCount = 0;
	  totArgCount = 0;
	  argCount = 0;
	}

	minLineLen = sizeof(inBuf);
	maxLineLen = 0;
	totLineLen = 0;
      }
#endif
      lineCount = 0;
      reload = FALSE;
    }

#ifdef DEBUG
    if ( config->debug ) {
      lineLen = strlen( inBuf );
      totLineLen += lineLen;
      if ( lineLen < minLineLen )
	minLineLen = lineLen;
      else if ( lineLen > maxLineLen )
	maxLineLen = lineLen;
    }
#endif

    if ( config->debug >= 3 )
      printf( "DEBUG - Before [%s]", inBuf );

    if ( ( ret = parseLine( inBuf ) ) > 0 ) {

#ifdef DEBUG
      if ( config->debug ) {
	/* save arg count */
	totArgCount += ret;
	if ( ret < minArgCount )
	  minArgCount = ret;
	else if ( ret > maxArgCount )
	  maxArgCount = ret;
      }
#endif

      /* the first field is the generated template */
      getParsedField( oBuf, sizeof( oBuf ), 0 );

      if ( config->match ) {
	if ( templateMatches( oBuf ) )
	  printf( "%s", inBuf );
      } else {
	/* load it into the hash */
	if ( ( tmpRec = getHashRecord( templateHash, oBuf, strlen(oBuf)+1 ) ) EQ NULL ) { /* new template */

#ifdef DEBUG
	  if ( config->debug >= 3 )
	    printf( "%s||%s", oBuf, inBuf );
#endif

	  /* store line metadata */
	  tmpMd = (metaData_t *)XMALLOC( sizeof( metaData_t ) );
	  XMEMSET( tmpMd, 0, sizeof( metaData_t ) );
	  tmpMd->count = 1;
	  XSTRNCPY( tmpMd->lBuf, inBuf, LINEBUF_SIZE );

	  /* process arguments if clustering is enabled */
	  if ( config->cluster ) {
	    curFieldPtr = &tmpMd->head;
	    for( i = 1; i < ret; i++ ) {
	      getParsedField( inBuf, sizeof( inBuf ), i );

	      if ( config->chain ) {

		/* we don't know if the field is variable */
		/* only chain based on certain field types */
		/* XXX need to expand field types that can be chained */
		if ( ( inBuf[0] EQ 'i' ) | ( inBuf[0] EQ 'I' ) | ( inBuf[0] EQ 'm' ) ) {
		    
#ifdef DEBUG
		  if ( config->debug >= 3 )
		    printf( "DEBUG - Searching chains for field [%s]\n", inBuf );
#endif
		}

	      }

#ifdef DEBUG
	      if ( config->debug >= 4 )
		printf( "DEBUG - Storing argument [%s]\n", inBuf );
#endif

	      if ( *curFieldPtr EQ NULL ) {
		*curFieldPtr = (struct Fields_s *)XMALLOC(sizeof( struct Fields_s ));
		XMEMSET( *curFieldPtr, 0, sizeof( struct Fields_s ) );
	      }
	      insertBinTree( &(*curFieldPtr)->head, inBuf );
	      (*curFieldPtr)->count = 1;
#ifdef DEBUG
	      if ( config->debug )
		argCount++;
#endif
	      curFieldPtr = &(*curFieldPtr)->next;
	    }
	  }

	  /* stuff the new record into the hash */
	  addUniqueHashRec( templateHash, oBuf, strlen(oBuf)+1, tmpMd );

	} else {

	  if ( tmpRec->data != NULL ) {
	    tmpMd = (metaData_t *)tmpRec->data;
	    tmpMd->count++;

#ifdef DEBUG
	    if ( config->debug >= 3 )
	      printf( "DEBUG - Updating existing template\n" );
#endif

	    /* update the argument info if clustering is enabled */
	    if ( config->cluster ) {
	      curFieldPtr = &tmpMd->head;
	      for( i = 1; i < ret; i++ ) {
		getParsedField( inBuf, sizeof( inBuf ), i );

#ifdef DEBUG
		if ( config->debug >= 4 )
		  printf( "DEBUG - Processing argument [%s]\n", inBuf );
#endif

		if ( *curFieldPtr EQ NULL ) {
		  *curFieldPtr = (struct Fields_s *)XMALLOC(sizeof( struct Fields_s ));
		  XMEMSET( *curFieldPtr, 0, sizeof( struct Fields_s ) );
		}

		if ( config->chain ) {
		  if ( (*curFieldPtr)->count > 1 ) { /* variable field */

		    /* only chain based on certain field types */
		    /* XXX need to expant field types that can be chained */
		    if ( ( inBuf[0] EQ 'i' ) || ( inBuf[0] EQ 'I' ) || ( inBuf[0] EQ 'm' ) ) {
		    
#ifdef DEBUG
		      if ( config->debug >= 3 )
			printf( "DEBUG - Searching chains for variable field [%s]\n", inBuf );
#endif
		    }
		  }
		}

		if ( (*curFieldPtr)->count <= config->clusterDepth ) {
		  if ( searchBinTree( (*curFieldPtr)->head, inBuf ) EQ NULL ) {
		    insertBinTree( &(*curFieldPtr)->head, inBuf );
		    (*curFieldPtr)->count++;
#ifdef DEBUG
		    if ( config->debug )
		      argCount++;
#endif
		  }
		}
		curFieldPtr = &(*curFieldPtr)->next;
	      }
	    }
	  }
	}
	templateHash = dyGrowHash( templateHash );
      }
      lineCount++;
    }
  }
  
#ifdef DEBUG
  if ( config->debug ) {
    fprintf( stderr, "Line length: min=%d, max=%d, avg=%2.0f\n", minLineLen, maxLineLen, (float)totLineLen / (float)lineCount );
      
    if ( config->cluster ) {
      fprintf( stderr, "Stored %d args/min\n", argCount );
      fprintf( stderr, "Arg count: min/%d, max/%d, avg/%2.0f\n", minArgCount, maxArgCount, (float)totArgCount / (float)lineCount );
      minArgCount = MAX_FIELD_POS;
      maxArgCount = 0;
      totArgCount = 0;
      argCount = 0;
    }
      
    minLineLen = sizeof(inBuf);
    maxLineLen = 0;
    totLineLen = 0;
  }
#endif
  
  if ( inFile != stdin )  
    fclose( inFile );
    
  deInitParser();

  return( EXIT_SUCCESS );
}

/****
 *
 * print templates
 *
 ****/

int showTemplates( void ) {

#ifdef DEBUG
  if ( config->debug >= 1 )
    printf( "DEBUG - Finished processing file, printing\n" );
#endif

  if ( templateHash != NULL ) {
    /* dump the template data */
    if ( traverseHash( templateHash, printTemplate ) EQ TRUE ) {
      freeHash( templateHash );
      return( EXIT_SUCCESS);
    }
    freeHash( templateHash );
  }

  return( EXIT_FAILURE );
}

/****
 *
 * load templates
 *
 ****/

int loadTemplateFile( const char *fName ) {
  FILE *inFile;
  char inBuf[8192];
  size_t count = 0;
  int lLen, i;

  /* init the hash if we need to */
  if ( templateHash EQ NULL )
    templateHash = initHash( 52 );

#ifdef DEBUG
  if ( config->debug >= 1 )
    printf( "DEBUG - Loading template file [%s]\n", fName );
#endif

  if ( ( inFile = fopen( fName, "r" ) ) EQ NULL ) {
    fprintf( stderr, "ERR - Unable to open template file [%s]\n", fName );
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
	printf( "DEBUG - Loading template [%s]\n", inBuf );
#endif

      count++;
      addUniqueHashRec( templateHash, inBuf, strlen( inBuf ), NULL );
    }
  }

  fclose( inFile );

#ifdef DEBUG
  if ( config->debug >= 1 )
    printf( "DEBUG - Loaded [%lu] templates from ignore file\n", count );
#endif

  return( TRUE );
}
