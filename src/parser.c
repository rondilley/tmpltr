/*****
 *
 * Description: Line Parser Functions
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
 * defines
 *
 ****/

/****
 * 
 * includes
 *
 ****/

#include "parser.h"

/****
 * 
 * local variables
 *
 ****/

PRIVATE char *fields[MAX_FIELD_POS];

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
 * init parser
 *
 ****/

void initParser( void ) {
  /* make sure the field list of clean */
  XMEMSET( fields, 0, sizeof( char * ) * MAX_FIELD_POS );
  
  /* XXX it would be faster to init all mem here instead of on-demand */
}

/****
 * 
 * de-init parser
 *
 ****/

void deInitParser( void ) {
  int i;
  
  for( i = 0; i < MAX_FIELD_POS; i++ )
    if ( fields[i] != NULL )
      XFREE( fields[i] );
}

/****
 * 
 * parse that line
 *
 * pass a line to the function and the function will
 * return a printf style format string
 *
 ****/

int parseLine( char *line ) {
  int curFormPos = 0;
  int curLinePos = 0;
  int startOfField, startOfOctet;
  int octet = 0, octetLen = 0;
  int curFieldType = FIELD_TYPE_UNDEF;
  int runLen = 0;
  int i;
  char *posPtr;
  char *key = NULL;
  char *tmpString;
  long tmpOffset;
  long offsetList[1024];
  int fieldPos = 0; // 0 is where we store the template
  int offsetPos = 0;
  int templatePos = 0;
  long tmpLongNum = 0;
  int tmpNum = 0;
  int inQuotes = FALSE;
  char fieldTypeChar;

  if ( fields[fieldPos] EQ NULL ) {
    if( ( fields[fieldPos] = (char *)XMALLOC( MAX_FIELD_LEN ) ) EQ NULL ) {
      display( LOG_ERR, "Unable to allocate memory for string" );
      return( 0 );
    }
  }
  fieldPos++;
  
  while( line[curLinePos] != '\0' ) {
    
    if ( runLen >= MAX_FIELD_LEN ) {

      fprintf( stderr, "ERR - Field is too long\n" );
      return( fieldPos-1 );

    } else if ( fieldPos >= MAX_FIELD_POS ) {

      fprintf( stderr, "ERR - Too many fields in line\n" );
      return( fieldPos-1 );

    } else if ( curFieldType EQ FIELD_TYPE_EXTRACT ) {

      if ( fields[fieldPos] EQ NULL ) {
	if( ( fields[fieldPos] = (char *)XMALLOC( MAX_FIELD_LEN ) ) EQ NULL ) {
	  fprintf( stderr, "ERR - Unable to allocate memory for string\n" );
	  return( fieldPos-1 );
	}
      }
      fields[fieldPos][runLen+1] = '\0';
      fields[fieldPos][0] = fieldTypeChar;
      XMEMCPY( fields[fieldPos]+1, line + startOfField, runLen );
      
#ifdef DEBUG
      if ( config->debug >= 5 ) {
	switch ( fieldTypeChar ) {
	case 's':
	  printf( "DEBUG - Extracted string [%s]\n", fields[fieldPos] );
	  break;
	case 'd':
	  printf( "DEBUG - Extracted number [%s]\n", fields[fieldPos] );
	  break;
	case 'c':
	  printf( "DEBUG - Extracted character [%s]\n", fields[fieldPos] );
	  break;
	case 'i':
	  printf( "DEBUG - Extracted ipv4 [%s]\n", fields[fieldPos] );
	  break;
	case 'I':
	  printf( "DEBUG - Extracted ipv6 [%s]\n", fields[fieldPos] );
	  break;
	case 'm':
	  printf( "DEBUG - Extracted MAC [%s]\n", fields[fieldPos] );
	  break;
	case 'x':
	  printf( "DEBUG - Extracted hex [%s]\n", fields[fieldPos] );
	  break;
	default:
	  printf( "DEBUG - Extracted unknown [%c] - [%s]\n", fieldTypeChar, fields[fieldPos] );
	  break;
	}
      }
#endif
	  
      /* update template */
      if ( templatePos > ( MAX_FIELD_LEN - 3 ) ) {
	fprintf( stderr, "ERR - Template is too long\n" );
	return( fieldPos-1 );
      }
      fields[0][templatePos++] = '%';
      fields[0][templatePos++] = fieldTypeChar;
      fields[0][templatePos] = '\0';
      fieldPos++;
	  
      /* switch field state */
      curFieldType = FIELD_TYPE_UNDEF;

    } else if ( curFieldType EQ FIELD_TYPE_STRING ) {
      
      /****
       * 
       * string
       *
       ****/
      
      if ( isalnum( line[curLinePos] ) ) {
	
	/****
	 * 
	 * add alpha numberic char to string
	 *
	 ****/
	
	runLen++;
	curLinePos++;
      } else if ( ( line[curLinePos] EQ '.' ) |
		  ( (inQuotes) && (line[curLinePos] EQ ',') ) |
		  ( line[curLinePos] EQ '-' ) |
		  ( (inQuotes) && (line[curLinePos] EQ ':') ) |
		  ( (inQuotes) && (line[curLinePos] EQ ';') ) |
		  ( (inQuotes) && (line[curLinePos] EQ '+') ) |
		  ( (inQuotes) && (line[curLinePos] EQ '!') ) |
		  ( (inQuotes) && (line[curLinePos] EQ '/') ) |
		  ( line[curLinePos] EQ '#' ) |
		  ( line[curLinePos] EQ '$' ) |
		  ( (inQuotes) && (line[curLinePos] EQ ' ') ) |
		  ( (inQuotes) && (line[curLinePos] EQ '(') ) |
		  ( (inQuotes) && (line[curLinePos] EQ ')') ) |
		  ( line[curLinePos] EQ '~' ) |
		  ( line[curLinePos] EQ '@' ) |
		  ( line[curLinePos] EQ '\\' ) |
		  ( line[curLinePos] EQ '_' )
		  ) {
	
	/****
	 * 
	 * add some printable characters to the string
	 *
	 ****/
	
	runLen++;
	curLinePos++;
	
      } else if ( line[curLinePos] EQ '%' ) {
	
	/****
	 * 
	 * add some printable characters to the string
	 *
	 ****/
	
	runLen++;
	curLinePos++;
	
      } else if ( ( line[curLinePos] EQ '\"' ) |
		  ( line[curLinePos] EQ '\'' ) ) {
	
	/****
	 * 
	 * deal with quoted fields, spaces and some printable characters
	 * will be added to the string
	 *
	 ****/
	
	/* check to see if it is the start or end */
	
	if ( inQuotes | config->greedy ) {
	  
	  /* extract string */
	  
	  if ( fields[fieldPos] EQ NULL ) {
	    if( ( fields[fieldPos] = (char *)XMALLOC( MAX_FIELD_LEN ) ) EQ NULL ) {
	      fprintf( stderr, "ERR - Unable to allocate memory for string\n" );
	      return( fieldPos-1 );
	    }
	  }
	  fields[fieldPos][runLen+1] = '\0';
	  fields[fieldPos][0] = 's'; 
	  XMEMCPY( fields[fieldPos]+1, line + startOfField, runLen );
	  
#ifdef DEBUG
	  if ( config->debug >= 5 )
	    printf( "DEBUG - Extracting string [%s]\n", fields[fieldPos] );
#endif
	  
	  /* update template */
	  if ( templatePos > ( MAX_FIELD_LEN - 4 ) ) {
	    fprintf( stderr, "ERR - Template is too long\n" );
	    return( fieldPos-1 );
	  }
	  fields[0][templatePos++] = '%';
	  fields[0][templatePos++] = 's';
	  fields[0][templatePos++] = line[curLinePos];
	  fields[0][templatePos] = 0;
	  
	  fieldPos++;
	  
	  /* switch field state */
	  curFieldType = FIELD_TYPE_UNDEF;
	  runLen = 1;
	  startOfField = ++curLinePos;
	  inQuotes = FALSE;
	  
	} else {
	  
	  /* at the start */
	  inQuotes = TRUE;
	  runLen++;
	  curLinePos++;
	}
	
      } else if ( ( line[curLinePos] EQ ':' ) |
		  ( line[curLinePos] EQ ' ' ) |
		  ( line[curLinePos] EQ '\t' ) |
		  ( line[curLinePos] EQ '=' ) ) {
	
	/****
	 * 
	 * if these characters are in quotes, treat it as a delimeter, if not, add it to the string
	 *
	 ****/
	
	if ( inQuotes ) {
	  
	  /* just add it to the string */
	  
	  runLen++;
	  curLinePos++;
	  
	} else {
	  
	  /* treat it as a delimeter */

	  /* extract field */
	  fieldTypeChar = 's';
	  curFieldType = FIELD_TYPE_EXTRACT;

	}
	
      } else if ( ispunct( line[curLinePos] ) ) {
	
	/****
	 * 
	 * punctuation is a delimeter
	 *
	 ****/
	
	if ( curLinePos > 0 ) {
	  if ( ( line[curLinePos-1] EQ ' ' ) | ( line[curLinePos-1] EQ '\t' ) ) {
	    /* last char was a blank */
	    runLen--;
	  }
	}
	
	/* extract field */
	fieldTypeChar = 's';
	curFieldType = FIELD_TYPE_EXTRACT;
	
      } else if ( ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) ) {
	
	/****
	 * 
	 * ignore control and non-printable characters
	 *
	 ****/
	
	/* extract field */
	fieldTypeChar = 's';
	curFieldType = FIELD_TYPE_EXTRACT;
	
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_CHAR ) {
      
      /****
       * 
       * char field
       *
       ****/
      
      if ( isalnum( line[curLinePos] ) |
	   ( line[curLinePos] EQ '/' ) |
	   ( line[curLinePos] EQ '@' ) |
	   ( ( inQuotes ) && ( line[curLinePos] EQ ' ' ) ) |
	   ( line[curLinePos] EQ '\\' ) |
	   ( line[curLinePos] EQ ' ' ) |
	   ( line[curLinePos] EQ '-' ) |
	   ( line[curLinePos] EQ ':' )
	   ) {

	/* convery char to string */
	curFieldType = FIELD_TYPE_STRING;
	runLen++;
	curLinePos++;

#ifdef HAVE_ISBLANK
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) |
		  ( isblank( line[curLinePos] ) ) ) {
#else
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ ' ' ) |
		  ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ '\t' ) ) {
#endif
	
	/* extract field */
	fieldTypeChar = 'c';
	curFieldType = FIELD_TYPE_EXTRACT;
	
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_IP4 ) {
      
      /****
       * 
       * IPv4 Address (xxx.xxx.xxx.xxx)
       * 
       ****/
      
      /* XXX need to add code to handle numbers beginning with 0 */
      if ( isdigit( line[curLinePos] ) && ( octetLen < 3 ) ) {
	
	runLen++;
	curLinePos++;
	octetLen++;
	
      } else if ( ( octet < 3 ) && ( line[curLinePos] EQ '.' ) ) {
	
	if ( ( octetLen > 0 ) && ( octetLen <= 3 ) && ( atoi( line + startOfOctet ) < 256 ) ) { /* is the octet valid */
	  
	  /* convert field to IPv4 */
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet++;
	  octetLen = 0;
	  
	} else {
	  /* not a valid ipv4 octet */
	  curFieldType = FIELD_TYPE_STRING;
	  runLen++;
	  curLinePos++;
	}
	
      } else if ( octet EQ 3 ) {
	
	if ( ( octetLen > 0 ) && ( octetLen <= 3 ) && ( atoi( line + startOfOctet ) < 256 ) ) { /* is the octet valid */
	  
	  /* extract field */
	  fieldTypeChar = 'i';
	  curFieldType = FIELD_TYPE_EXTRACT;

	} else {
	  /* last octec is invalid */
	  curFieldType = FIELD_TYPE_STRING;
	}
      } else {
	curFieldType = FIELD_TYPE_STRING;
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_IP6 ) {
      
      /****
       * 
       * IPv6 Address (xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx, xxxx:xxxx:xxxx:xxxx::)
       * 
       ****/
      
      /* XXX need to add code to handle numbers beginning with 0 */
      if ( isxdigit( line[curLinePos] ) && ( octetLen < 4 ) ) {
	runLen++;
	curLinePos++;
	octetLen++;
	
      } else if ( ( octet < 8 ) && ( line[curLinePos] EQ ':' ) ) {
	
	if ( ( octetLen > 0 ) && ( octetLen <= 4 ) ) { /* is the octet valid */
	  
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet++;
	  octetLen = 0;
	  
	} else {
	  
	  /* not a valid ipv6 octet */
	  /* XXX need to add rollback so we dont loose previous fields */
	  curFieldType = FIELD_TYPE_STRING;
	  runLen++;
	  curLinePos++;
	}
	
      } else if ( octet EQ 7 ) {
	
	if ( ( octetLen > 0 ) && ( octetLen <= 4 ) ) { /* is the octet valid */
	  
	  /* extract field */
	  fieldTypeChar = 'I';
	  curFieldType = FIELD_TYPE_EXTRACT;

	} else {
	  /* last octec is invalid */
	  curFieldType = FIELD_TYPE_STRING;
	}
      } else {
	curFieldType = FIELD_TYPE_STRING;
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_MACADDR ) {
      
      /****
       * 
       * MAC Address (xx:xx:xx:xx:xx:xx or xx-xx-xx-xx-xx-xx)
       * 
       ****/
      
      /* XXX need to add code to handle numbers beginning with 0 */
      if ( isxdigit( line[curLinePos] ) && ( octetLen < 2 ) ) {
	runLen++;
	curLinePos++;
	octetLen++;
	
      } else if ( ( octet < 5 ) && ( ( line[curLinePos] EQ ':' ) || line[curLinePos] EQ '-' ) ) {
	
	if ( octetLen EQ 2 ) { /* is the octet valid */
	  
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet++;
	  octetLen = 0;
	  
	} else {
	  
	  /* not a valid ipv6 octet */
	  /* XXX need to add rollback so we dont loose previous fields */
	  curFieldType = FIELD_TYPE_STRING;
	  runLen++;
	  curLinePos++;
	}
	
      } else if ( octet EQ 5 ) {
	
	if ( octetLen EQ 2 ) { /* is the octet valid */
	  
	  /* extract field */
	  fieldTypeChar = 'm';
	  curFieldType = FIELD_TYPE_EXTRACT;

	} else {
	  /* last octec is invalid */
	  curFieldType = FIELD_TYPE_STRING;
	}
      } else {
	curFieldType = FIELD_TYPE_STRING;
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_NUM_INT ) {
      
      /****
       * 
       * number field
       *
       ****/
      
      /* XXX need to add code to handle numbers beginning with 0 */
      if ( isdigit( line[curLinePos] ) ) {
	
	runLen++;
	curLinePos++;
	
      } else if ( isxdigit( line[curLinePos] ) ) {
	
	/* convert field to hex */
	curFieldType = FIELD_TYPE_NUM_HEX;
	runLen++;
	curLinePos++;
	
      } else if ( isalpha( line[curLinePos] ) |
		  ( line[curLinePos] EQ '@' ) |
		  ( (inQuotes) && ( line[curLinePos] EQ ' ' ) ) |
		  ( line[curLinePos] EQ '\\' )
		  ) {
	/* convert field to string */
	curFieldType = FIELD_TYPE_STRING;
	runLen++;
	curLinePos++;
	
      } else if ( line[curLinePos] EQ '.' ) {
	
	if ( ( runLen <= 3 ) & ( atoi( line + startOfField ) < 256 ) ) { /* check to see if this is the start of an IP address */
	  
	  /* convert field to IPv4 */
	  curFieldType = FIELD_TYPE_IP4;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;
	  
	} else {

	  /* extract field */
	  fieldTypeChar = 'd';
	  curFieldType = FIELD_TYPE_EXTRACT;

	}
	
      } else if ( line[curLinePos] EQ ':' ) {
	
	if ( runLen EQ 2 ) {
	  
	  /* convert field to MAC */
	  curFieldType = FIELD_TYPE_MACADDR;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;
	  
	} else if ( runLen EQ 4 ) {
	  
	  /* convert field to IPv6 */
	  curFieldType = FIELD_TYPE_IP6;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;
	  
	} else {

	  /* extract field */
	  fieldTypeChar = 'd';
	  curFieldType = FIELD_TYPE_EXTRACT;

	}

      } else if ( line[curLinePos] EQ '-' ) {
	
	if ( runLen EQ 2 ) {
	  
	  /* convert field to MAC */
	  curFieldType = FIELD_TYPE_MACADDR;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;
	  
	} else {

	  /* extract field */
	  fieldTypeChar = 'd';
	  curFieldType = FIELD_TYPE_EXTRACT;

	}
	
#ifdef HAVE_ISBLANK
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( isblank( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ '/' ) |
		  ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) ) {
#else
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ ' ' ) |
		  ( line[curLinePos] EQ '\t' ) |
		  ( line[curLinePos] EQ '/' ) |
		  ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) ) {
#endif
	
	/* extract field */
	fieldTypeChar = 'd';
	curFieldType = FIELD_TYPE_EXTRACT;
	
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_NUM_FLOAT ) {
      
      /****
       * 
       * float
       *
       ****/
      
    } else if ( curFieldType EQ FIELD_TYPE_NUM_HEX ) {
      
      /****
       * 
       * hex field
       *
       ****/
      
      /* XXX need to add code to handle hex numbers beginning with 0x */
      /* XXX need to add code to handle numbers beginning with 0 */
      if ( isxdigit( line[curLinePos] ) ) {
	runLen++;
	curLinePos++;
	
      } else if ( isalpha( line[curLinePos] ) |
		  ( line[curLinePos] EQ '@' ) |
		  ( (inQuotes) && ( line[curLinePos] EQ ' ' ) ) |
		  ( line[curLinePos] EQ '\\' )
		  ) {
	/* convert field to string */
	curFieldType = FIELD_TYPE_STRING;
	runLen++;
	curLinePos++;
	
      } else if ( line[curLinePos] EQ ':' ) {
	
	if ( runLen EQ 4 ) { /* check to see if this is the start of an IPv6 address */
	  
	  /* convert field to IPv4 */
	  curFieldType = FIELD_TYPE_IP6;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;
	  
	} else if ( runLen EQ 2 ) { /* check to see if this is the start of a MAC address */
	  
	  /* convert field to MAC Address */
	  curFieldType = FIELD_TYPE_MACADDR;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;

	} else {

	  /* extract field */
	  fieldTypeChar = 'x';
	  curFieldType = FIELD_TYPE_EXTRACT;

	}
	
      } else if ( line[curLinePos] EQ '-' ) {
	
	if ( runLen EQ 2 ) { /* check to see if this is the start of a MAC address */
	  
	  /* convert field to MAC Address */
	  curFieldType = FIELD_TYPE_MACADDR;
	  runLen++;
	  startOfOctet = ++curLinePos;
	  octet = 1;
	  octetLen = 0;

	} else {

	  /* extract field */
	  fieldTypeChar = 'x';
	  curFieldType = FIELD_TYPE_EXTRACT;

	}
	
#ifdef HAVE_ISBLANK
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( isblank( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ '/' ) |
		  ( line[curLinePos] EQ '.' ) |
		  ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) ) {
#else
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ ' ' ) |
		  ( line[curLinePos] EQ '\t' ) |
		  ( line[curLinePos] EQ '/' ) |
		  ( line[curLinePos] EQ '.' ) |
		  ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) ) {
#endif
	
	/* extract field */
	fieldTypeChar = 'x';
	curFieldType = FIELD_TYPE_EXTRACT;
	
      }
      
    } else if ( curFieldType EQ FIELD_TYPE_STATIC ) {
      
      /****
       * 
       * printable, but non-alphanumeric
       *
       ****/
      
      /* this is a placeholder for figuring out how to handle multiple spaces */
      
      curFieldType = FIELD_TYPE_UNDEF;
      
    } else {
      
      /****
       * 
       * begining of new field
       *
       ****/
      
      if ( isdigit( line[curLinePos] ) ) {
	curFieldType = FIELD_TYPE_NUM_INT;
	runLen = 1;
	startOfField = curLinePos++;
      } else if ( isxdigit( line[curLinePos] ) ) {
	curFieldType = FIELD_TYPE_NUM_HEX;
	runLen = 1;
	startOfField = curLinePos++;
      } else if ( isalpha( line[curLinePos] ) |
		  ( ( inQuotes ) && ( line[curLinePos] EQ '/' ) ) |
		  ( line[curLinePos] EQ '@' ) |
		  ( line[curLinePos] EQ '%' ) |
		  ( line[curLinePos] EQ '$' ) |
		  ( line[curLinePos] EQ '\\' )
		  ) {
	curFieldType = FIELD_TYPE_CHAR;
	runLen = 1;
	startOfField = curLinePos++;
	
      } else if ( ( line[curLinePos] EQ '\"' ) |
		  ( line[curLinePos] EQ '\'' ) ) {
	if ( inQuotes ) {
	  /* something is really broke */
	  runLen++;
	  curLinePos++;
	  inQuotes = FALSE;
	} else {
	  if ( !config->greedy ) {
	    if ( templatePos > ( MAX_FIELD_LEN - 2 ) ) {
	      fprintf( stderr, "ERR - Template is too long\n" );
	      return( fieldPos-1 );
	    }
	    fields[0][templatePos++] = line[curLinePos];
	    fields[0][templatePos] = '\0';
	    curFieldType = FIELD_TYPE_STRING;
	    inQuotes = TRUE;
	    runLen = 0;
	    startOfField = ++curLinePos;
	  } else {
	    /* printable but not alpha+num */
	    if ( templatePos > ( MAX_FIELD_LEN - 2 ) ) {
	      fprintf( stderr, "ERR - Template is too long\n" );
	      return( fieldPos-1 );
	    }
	    fields[0][templatePos++] = line[curLinePos];
	    fields[0][templatePos] = '\0';
#ifdef DEBUG
	    if ( config->debug >= 10 )
	      printf( "DEBUG - Updated template [%s]\n", fields[0] );
#endif
	    curFieldType = FIELD_TYPE_STATIC;
	    runLen = 1;
	    startOfField = curLinePos++;
	  }
	}
      } else if ( ( iscntrl( line[curLinePos] ) ) | !( isprint( line[curLinePos] ) ) ) {
	/* not a valid log character, ignore it for now */
	curLinePos++;
#ifdef HAVE_ISBLANK
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( isblank( line[curLinePos] ) ) |
		  ( isprint( line[curLinePos] ) ) ) {
#else
      } else if ( ( ispunct( line[curLinePos] ) ) |
		  ( isprint( line[curLinePos] ) ) |
		  ( line[curLinePos] EQ ' ' ) |
		  ( line[curLinePos] EQ '\t' ) ) {
#endif
	/* printable but not alpha+num */
	if ( templatePos > ( MAX_FIELD_LEN - 2 ) ) {
	  fprintf( stderr, "ERR - Template is too long\n" );
	  return( fieldPos-1 );
	}
	fields[0][templatePos++] = line[curLinePos];
	fields[0][templatePos] = '\0';
#ifdef DEBUG
	if ( config->debug >= 10 )
	  printf( "DEBUG - Updated template [%s]\n", fields[0] );
#endif
	curFieldType = FIELD_TYPE_STATIC;
	runLen = 1;
	startOfField = curLinePos++;
      } else {
	/* ignore it */
	curLinePos++;
      }
    }
  }
  
  /* just in case the line was 0 length */
  if ( curLinePos EQ 0 )
    return( 0 );
  
  return( fieldPos );
}

/****
 * 
 * return parsed field
 *
 ****/

int getParsedField( char *oBuf, int oBufLen, const unsigned int fieldNum ) {
  if ( ( fieldNum >= MAX_FIELD_POS ) || ( fields[fieldNum] EQ NULL ) ) {
    fprintf( stderr, "ERR - Requested field does not exist [%d]\n", fieldNum );
    oBuf[0] = 0;
    return( FAILED );
  }
  XSTRNCPY( oBuf, fields[fieldNum], oBufLen );
  return( TRUE );
}

