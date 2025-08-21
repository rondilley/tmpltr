/*****
 *
 * Description: Line Parser Functions
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

#ifdef DEBUG
PRIVATE size_t count_extract = 0;
PRIVATE size_t count_string = 0;
PRIVATE size_t count_char = 0;
PRIVATE size_t count_ip4 = 0;
PRIVATE size_t count_ip6 = 0;
PRIVATE size_t count_mac = 0;
PRIVATE size_t count_num_int = 0;
PRIVATE size_t count_num_float = 0;
PRIVATE size_t count_num_hex = 0;
PRIVATE size_t count_dt = 0;
PRIVATE size_t count_dt_syslog = 0;
PRIVATE size_t count_dt_epoch = 0;
PRIVATE size_t count_dt_iso8601 = 0;
PRIVATE size_t count_dt_apache = 0;
PRIVATE size_t count_url = 0;
PRIVATE size_t count_base64 = 0;
PRIVATE size_t count_static = 0;
#endif

/* Month name lookup table for syslog format */
PRIVATE const char *month_names[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/* Helper function to check if string matches a month name */
PRIVATE int isMonthName(const char *str, int len)
{
  int i;
  if (len != 3)
    return FALSE;

  for (i = 0; i < 12; i++)
  {
    if (strncmp(str, month_names[i], 3) == 0)
      return TRUE;
  }
  return FALSE;
}

/* Base64 character lookup table for O(1) validation */
/* Valid chars: A-Z (65-90), a-z (97-122), 0-9 (48-57), + (43), / (47), = (61) */
PRIVATE const unsigned char base64_table[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0-15 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 16-31 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, /* 32-47: + at 43, / at 47 */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 0, 0, /* 48-63: 0-9, = at 61 (2=padding) */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 64-79: A-O */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* 80-95: P-Z */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 96-111: a-o */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* 112-127: p-z */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 128-143 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 144-159 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 160-175 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 176-191 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 192-207 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 208-223 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 224-239 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 240-255 */
};

/* Helper function to check for valid Base64 character - inline for speed */
PRIVATE inline int isBase64Char(unsigned char c)
{
  return base64_table[c];
}

/* Helper function to check if we're at the start of a syslog date pattern */
PRIVATE int isSyslogDate(const char *line, int pos, int lineLen)
{
  /* Pattern: "Mmm dd hh:mm:ss" or "Mmm  d hh:mm:ss" */
  /* Minimum length needed: 15 characters (double digit day) */
  if (pos + 15 > lineLen)
    return FALSE;

  /* Check month name (3 chars) */
  if (!isMonthName(line + pos, 3))
    return FALSE;

  /* Check space after month */
  if (line[pos + 3] != ' ')
    return FALSE;

  /* Check day (1 or 2 digits with possible leading space) */
  int dayPos = pos + 4;
  if (line[dayPos] == ' ')
  {
    /* Single digit day with leading space */
    dayPos++;
    if (!FAST_ISDIGIT(line[dayPos]))
      return FALSE;
  }
  else if (FAST_ISDIGIT(line[dayPos]))
  {
    /* Two digit day */
    if (!FAST_ISDIGIT(line[dayPos + 1]))
      return FALSE;
    dayPos++;
  }
  else
  {
    return FALSE;
  }

  /* Check space before time */
  dayPos++;
  if (line[dayPos] != ' ')
    return FALSE;

  /* Check time pattern: hh:mm:ss */
  int timePos = dayPos + 1;
  if (!FAST_ISDIGIT(line[timePos]) || !FAST_ISDIGIT(line[timePos + 1]))
    return FALSE;
  if (line[timePos + 2] != ':')
    return FALSE;
  if (!FAST_ISDIGIT(line[timePos + 3]) || !FAST_ISDIGIT(line[timePos + 4]))
    return FALSE;
  if (line[timePos + 5] != ':')
    return FALSE;
  if (!FAST_ISDIGIT(line[timePos + 6]) || !FAST_ISDIGIT(line[timePos + 7]))
    return FALSE;

  return TRUE;
}

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
 * Initialize parser with pre-allocated field storage
 *
 * DESCRIPTION:
 *   Initializes the log line parser by pre-allocating memory for all
 *   field storage arrays. This optimization eliminates malloc overhead
 *   during parsing by allocating all required memory upfront.
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   void
 *
 * SIDE EFFECTS:
 *   - Allocates memory for MAX_FIELD_POS fields of MAX_FIELD_LEN each
 *   - Initializes all fields to empty strings
 *   - On allocation failure, cleans up partial allocations and exits
 *
 * MEMORY ALLOCATION:
 *   Allocates MAX_FIELD_POS * MAX_FIELD_LEN bytes total
 *   All fields are zero-initialized for clean state
 *
 * ERROR HANDLING:
 *   If allocation fails, frees any successfully allocated fields
 *   and logs error message before returning
 *
 ****/

void initParser(void)
{
  int i;

  /* make sure the field list of clean */
  XMEMSET(fields, 0, sizeof(char *) * MAX_FIELD_POS);

  /* Pre-allocate all field storage for better performance */
  for (i = 0; i < MAX_FIELD_POS; i++)
  {
    if ((fields[i] = (char *)XMALLOC(MAX_FIELD_LEN)) == NULL)
    {
      /* Cleanup partial allocation on failure */
      for (int j = 0; j < i; j++)
      {
        XFREE(fields[j]);
        fields[j] = NULL;
      }
      display(LOG_ERR, "Unable to pre-allocate parser field storage");
      return;
    }
    /* Initialize to empty string */
    fields[i][0] = '\0';
  }
}

/****
 *
 * Deinitialize parser and free field storage
 *
 * DESCRIPTION:
 *   Cleans up parser state by freeing all pre-allocated field storage
 *   memory. Should be called when parser is no longer needed to
 *   prevent memory leaks.
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   void
 *
 * SIDE EFFECTS:
 *   Frees all memory allocated for field arrays in initParser()
 *
 * PRECONDITIONS:
 *   initParser() should have been called previously
 *
 * POSTCONDITIONS:
 *   All field pointers are freed, parser is ready for re-initialization
 *
 ****/

void deInitParser(void)
{
  int i;

  for (i = 0; i < MAX_FIELD_POS; i++)
  {
    if (fields[i] != NULL)
    {
      XFREE(fields[i]);
      fields[i] = NULL;
    }
  }
}

/****
 *
 * parse that line
 *
 * pass a line to the function and the function will
 * return a printf style format string
 *
 ****/

int parseLine(char *line)
{
  int curLinePos = 0;
  int startOfField, startOfOctet;
  int octet = 0, octetLen = 0;
  int curFieldType = FIELD_TYPE_UNDEF;
  int runLen = 0;
  int fieldPos = 0; // 0 is where we store the template
  int templatePos = 0;
  int inQuotes = FALSE;
  char fieldTypeChar;
  char curChar = line[0];
  int lineLen = strlen(line);
  int base64BlockPos = 0;                /* Position within 4-byte block */
  int base64PaddingCount = 0;            /* Track '=' padding chars */
  int savedFieldType = FIELD_TYPE_UNDEF; /* For rollback */
  int hexCase = 0;                       /* 0=unset, 1=lower, 2=upper */
  int macCase = 0;                       /* 0=unset, 1=lower, 2=upper */

  /* Field 0 is pre-allocated for template storage */
  fieldPos++;

  while (curChar != '\0')
  {

    if (runLen >= MAX_FIELD_LEN - 1)
    {

      fprintf(stderr, "ERR - Field is too long\n");
      return (fieldPos - 1);
    }
    else if (fieldPos >= MAX_FIELD_POS)
    {

      fprintf(stderr, "ERR - Too many fields in line\n");
      return (fieldPos - 1);
    }
    else if (curFieldType == FIELD_TYPE_STRING)
    {
      /******************************************************************
       ****************************** STRING ****************************
       ******************************************************************/

#ifdef DEBUG
      count_string++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=string\n");
#endif

      if (FAST_ISALNUM(curChar))
      {
        runLen++;
        curLinePos++;
      }
      else
      {
        switch (curChar)
        {
        case '.':
        case '-':
        case '#':
        case '$':
        case '~':
        case '^':
        case '@':
        case '%':
        case '_':
        case '\\':
        case '*':
          runLen++;
          curLinePos++;
          break;
        case '\"':
          /* check to see if it is the start or end */

          if (inQuotes || config->greedy)
          {

            /* extract string - field already pre-allocated */
            fields[fieldPos][runLen + 1] = '\0';
            fields[fieldPos][0] = 's';
            XMEMCPY(fields[fieldPos] + 1, line + startOfField, runLen);

#ifdef DEBUG
            if (config->debug >= 5)
              printf("DEBUG - Extracting string [%s]\n", fields[fieldPos] + 1);
#endif

            /* update template */
            if (templatePos > (MAX_FIELD_LEN - 4))
            {
              fprintf(stderr, "ERR - Template is too long\n");
              return (fieldPos - 1);
            }
            fields[0][templatePos++] = '%';
            fields[0][templatePos++] = 's';
            fields[0][templatePos++] = curChar;
            fields[0][templatePos] = 0;

            fieldPos++;

            /* switch field state */
            curFieldType = FIELD_TYPE_UNDEF;
            runLen = 1;
            startOfField = ++curLinePos;
            inQuotes = FALSE;
          }
          else
          {
            /* at the start */
            inQuotes = TRUE;
            runLen++;
            curLinePos++;
          }

          break;
        case '>':
        case '<':
        case ',':
        case ':':
        case ';':
        case '+':
        case '!':
        case '/':
        case ' ':
        case '\t':
        case '=':
        case '?':
        case '{':
        case '}':
        case ']':
        case '[':
        case '\'':
        case '`':
        case '&':
        case '(':
        case ')':
        case '\r': // to address broken input fields
          if (inQuotes)
          {
            /* add some printable characters to the string if in quotes */
            runLen++;
            curLinePos++;
            break;
          }
          __attribute__((fallthrough));
        default:
          /* extract field */
          fieldTypeChar = 's';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
      }
    }
    else if (curFieldType == FIELD_TYPE_NUM_INT)
    {
      /******************************************************************
       ***************************** NUM_INT ****************************
       ******************************************************************/

#ifdef DEBUG
      count_num_int++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=num_int\n");
#endif

      /* Check for digits first to continue in NUM_INT state */
      if (FAST_ISDIGIT(curChar))
      {
        runLen++;
        curLinePos++;
      }
      else if (FAST_ISXDIGIT(curChar))
      {
        /* convert field to hex */
        curFieldType = FIELD_TYPE_NUM_HEX;
        runLen++;
        curLinePos++;
      }
      else
      {
        /* Use switch for single character decisions */
        switch (curChar)
        {
        case '.':
          if ((runLen <= 3) &&
              (atoi(line + startOfField) <
               256))
          { /* check to see if this is the start of an IP address */

            /* convert field to IPv4 */
            curFieldType = FIELD_TYPE_IP4;
            runLen++;
            startOfOctet = ++curLinePos;
            octet = 1;
            octetLen = 0;
          }
          else
          {
            /* convert field to float */
            curFieldType = FIELD_TYPE_NUM_FLOAT;
            runLen++;
            curLinePos++;
          }

          break;

        case '-':
          /* Check for special cases first */
          if ((runLen == 4) && (curLinePos + 12 <= lineLen))
          {
            /* look forward and see if this may be a date/time */
            /* XXX 2020-12-14 00:14:59.912 UTC */
            if (line[curLinePos + 3] == '-')
            {
              if (line[curLinePos + 6] == ' ')
              {
                if (line[curLinePos + 9] == ':')
                {
                  if (line[curLinePos + 12] == ':')
                  {
                    curFieldType = FIELD_TYPE_DT;
                    runLen++;
                    curLinePos++;
                  }
                  else
                  {
                    /* convert field to string */
                    curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                    runLen++;
                    curLinePos++;
                  }
                }
                else
                {
                  /* convert field to string */
                  curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                  runLen++;
                  curLinePos++;
                }
              }
              else
              {
                /* convert field to string */
                curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                runLen++;
                curLinePos++;
              }
            }
            else
            {
              /* convert field to string */
              curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
              runLen++;
              curLinePos++;
            }
          }
          else if (runLen == 2)
          {
            /* convert field to MAC */
            curFieldType = FIELD_TYPE_MACADDR;
            runLen++;
            startOfOctet = ++curLinePos;
            octet = 1;
            octetLen = 0;
          }
          else
          {
            /* extract field */
            fieldTypeChar = 'd';
            curFieldType = FIELD_TYPE_EXTRACT;
          }
          break;

        case ':':
          if (runLen == 2)
          {
            /* convert field to MAC */
            curFieldType = FIELD_TYPE_MACADDR;
            runLen++;
            startOfOctet = ++curLinePos;
            octet = 1;
            octetLen = 0;
          }
          else if (runLen == 4)
          {
            /* convert field to IPv6 */
            curFieldType = FIELD_TYPE_IP6;
            runLen++;
            startOfOctet = ++curLinePos;
            octet = 1;
            octetLen = 0;
          }
          else
          {
            /* extract field */
            fieldTypeChar = 'd';
            curFieldType = FIELD_TYPE_EXTRACT;
          }
          break;

        case '@':
        case '\\':
          /* convert field to string */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
          break;

        case ' ':
          if (inQuotes)
          {
            /* convert field to string */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
          }
          else
          {
            /* extract field */
            fieldTypeChar = 'd';
            curFieldType = FIELD_TYPE_EXTRACT;
          }
          break;

        case '\t':
        case '/':
          /* extract field */
          fieldTypeChar = 'd';
          curFieldType = FIELD_TYPE_EXTRACT;
          break;

        default:
          /* Handle alphabetic and other characters */
          if (FAST_ISALPHA(curChar))
          {
            /* convert field to string */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
          }
#ifdef HAVE_ISBLANK
          else if ((ispunct(curChar)) || (isblank(curChar)) ||
                   (iscntrl(curChar)) || !(isprint(curChar)))
#else
          else if ((ispunct(curChar)) || (iscntrl(curChar)) || !(isprint(curChar)))
#endif
          {
            /* extract field */
            fieldTypeChar = 'd';
            curFieldType = FIELD_TYPE_EXTRACT;
          }
          else
          {
            /* convert field to string */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
          }
          break;
        }
      }
    }

    else if (curFieldType == FIELD_TYPE_DT)
    {
      /******************************************************************
       ****************************** DATE/TIME *************************
       ******************************************************************/

#ifdef DEBUG
      count_dt++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=date\n");
#endif

      /* XXX 2020-12-14 00:14:59.912 UTC */
      /* already tested 4, 7, 10, 13, 16 */

#ifdef DEBUG
      if (config->debug >= 10)
        printf("DEBUG - time_rl: %d [%c]\n", runLen, curChar);
#endif

      switch (runLen)
      {
      case 7:
      case 10:
      case 13:
      case 16:
        runLen++;
        curLinePos++;
        break;
      case 5:
      case 6:
      case 8:
      case 9:
      case 11:
      case 12:
      case 14:
      case 15:
      case 17:
      case 18:
        if (FAST_ISDIGIT(curChar))
        {
          runLen++;
          curLinePos++;
        }
        else
        {
          /* convert field to string */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
        }
        break;
      case 19:
        if (curChar == '.')
        {
          /* date may include milliseconds */
          runLen++;
          curLinePos++;
        }
        else if (FAST_ISDIGIT(curChar))
        {
          /* too many digits in seconds, must be a string */
          /* convert field to string */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
        else
        {
          /* date is done, extract it */
          fieldTypeChar = 't';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        break;
      default:
        /* handle positions after offset 20 */
        if (FAST_ISDIGIT(curChar))
        {
          runLen++;
          curLinePos++;
        }
        else
        {
          /* date is done, extract it */
          fieldTypeChar = 't';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
      }
    }

    else if (curFieldType == FIELD_TYPE_DT_SYSLOG)
    {
      /******************************************************************
       ****************************** SYSLOG DATE ********************
       ******************************************************************/

#ifdef DEBUG
      count_static++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=syslog date\n");
#endif

      /* Pattern: "Mmm dd hh:mm:ss" or "Mmm  d hh:mm:ss" - total 15 chars */
      if (runLen < 15)
      {
        /* Continue consuming characters until we have the full date */
        runLen++;
        curLinePos++;
      }
      else
      {
        /* We've consumed the full syslog date, extract it */
        fieldTypeChar = 'D';
        curFieldType = FIELD_TYPE_EXTRACT;
      }
    }

    else if (curFieldType == FIELD_TYPE_EXTRACT)
    {
      /******************************************************************
       ****************************** EXTRACT ***************************
       ******************************************************************/

#ifdef DEBUG
      count_extract++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=extract\n");
#endif
      if (fields[fieldPos] == NULL)
      {
        if ((fields[fieldPos] = (char *)XMALLOC(MAX_FIELD_LEN)) == NULL)
        {
          fprintf(stderr, "ERR - Unable to allocate memory for string\n");
          return (fieldPos - 1);
        }
      }
      fields[fieldPos][runLen + 1] = '\0';
      fields[fieldPos][0] = fieldTypeChar;
      XMEMCPY(fields[fieldPos] + 1, line + startOfField, runLen);

#ifdef DEBUG
      if (config->debug >= 5)
      {
        switch (fieldTypeChar)
        {
        case 's':
          printf("DEBUG - Extracted string [%s]\n", fields[fieldPos] + 1);
          break;
        case 'd':
          printf("DEBUG - Extracted number [%s]\n", fields[fieldPos] + 1);
          break;
        case 'f':
          printf("DEBUG - Extracted float [%s]\n", fields[fieldPos] + 1);
          break;
        case 'c':
          printf("DEBUG - Extracted character [%s]\n", fields[fieldPos] + 1);
          break;
        case 'i':
          printf("DEBUG - Extracted ipv4 [%s]\n", fields[fieldPos] + 1);
          break;
        case 'I':
          printf("DEBUG - Extracted ipv6 [%s]\n", fields[fieldPos] + 1);
          break;
        case 'm':
          printf("DEBUG - Extracted MAC [%s]\n", fields[fieldPos] + 1);
          break;
        case 'x':
          printf("DEBUG - Extracted hex [%s]\n", fields[fieldPos] + 1);
          break;
        case 't':
          printf("DEBUG - Extracted date [%s]\n", fields[fieldPos] + 1);
          break;
        case 'D':
          printf("DEBUG - Extracted syslog date [%s]\n", fields[fieldPos] + 1);
          break;
        case 'b':
          printf("DEBUG - Extracted base64 [%s]\n", fields[fieldPos] + 1);
          break;
        default:
          printf("DEBUG - Extracted unknown [%c] - [%s]\n", fieldTypeChar,
                 fields[fieldPos]);
          break;
        }
      }
#endif

      /* update template */
      if (templatePos > (MAX_FIELD_LEN - 3))
      {
        fprintf(stderr, "ERR - Template is too long\n");
        return (fieldPos - 1);
      }
      fields[0][templatePos++] = '%';
      fields[0][templatePos++] = fieldTypeChar;
      fields[0][templatePos] = '\0';
      fieldPos++;

      /* switch field state */
      curFieldType = FIELD_TYPE_UNDEF;
    }
    else if (curFieldType == FIELD_TYPE_IP4)
    {
      /******************************************************************
       ****************************** IPv4 ******************************
       ******************************************************************/

#ifdef DEBUG
      count_ip4++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=IPv4\n");
#endif

      /* XXX need to add code to handle numbers beginning with 0 */
      if (isdigit(curChar))
      {
        runLen++;
        curLinePos++;
        octetLen++;
      }
      else if (curChar == '.')
      {
        if ((octet < 3) && (octetLen > 0) && (octetLen <= 3) &&
            (atoi(line + startOfOctet) < 256))
        { /* is the octet valid */
          runLen++;
          startOfOctet = ++curLinePos;
          octet++;
          octetLen = 0;
        }
        else
        {

          /* switch field to string */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
      }

      else if (octet == 3)
      {
        if ((octetLen > 0) && (octetLen <= 3) &&
            (atoi(line + startOfOctet) < 256))
        { /* is the octet valid */

          /* extract field */
          fieldTypeChar = 'i';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        else
        {
          /* last octec is invalid */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
        }
      }
      else
      {
        if (octet == 1)
        {
          /* extract field */
          fieldTypeChar = 'f';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        else
        {
          /* convert field to string */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
      }
    }

    else if (curFieldType == FIELD_TYPE_MACADDR)
    {
      /******************************************************************
       ****************************** MAC ADDR **************************
       ******************************************************************/

#ifdef DEBUG
      count_mac++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=MACADDR\n");
#endif

      /* MAC Address (xx:xx:xx:xx:xx:xx or xx-xx-xx-xx-xx-xx) */

      /* XXX need to add code to handle numbers beginning with 0 */
      if (FAST_ISXDIGIT(curChar) && (octetLen < 2))
      {
        /* Check case consistency for alphabetic hex digits in MAC */
        if (FAST_ISALPHA(curChar))
        {
          int currentCase = islower(curChar) ? 1 : 2;
          if (macCase == 0)
          {
            /* First alphabetic hex digit in MAC, set the case */
            macCase = currentCase;
          }
          else if (macCase != currentCase)
          {
            /* Case mismatch - convert to string */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
            macCase = 0; /* Reset for next field */
            break; /* Exit this case to process as string */
          }
        }
        runLen++;
        curLinePos++;
        octetLen++;
      }
      else if ((octet < 5) &&
               ((curChar == ':') || curChar == '-'))
      {

        if (octetLen == 2)
        { /* is the octet valid */

          runLen++;
          startOfOctet = ++curLinePos;
          octet++;
          octetLen = 0;
        }
        else
        {

          /* not a valid ipv6 octet */
          /* XXX need to add rollback so we dont loose previous fields */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
      }
      else if (octet == 5)
      {

        if (octetLen == 2)
        { /* is the octet valid */

          /* extract field */
          fieldTypeChar = 'm';
          curFieldType = FIELD_TYPE_EXTRACT;
          macCase = 0; /* Reset mac case */
        }
        else
        {
          /* last octec is invalid */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
        }
      }
      else
      {
        curFieldType = FIELD_TYPE_STRING;
        macCase = 0; /* Reset mac case */
      }
    }

    else if (curFieldType == FIELD_TYPE_CHAR)
    {
      /******************************************************************
       ****************************** CHAR ******************************
       ******************************************************************/

#ifdef DEBUG
      count_char++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=char\n");
#endif

      switch (curChar)
      {
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '/':
      case '@':
      case '\\':
      case '-':
      case ':':
      case '*':
        /* convert char to string */
        curFieldType = FIELD_TYPE_STRING;
        runLen++;
        curLinePos++;
        break;

      case ' ':
      case '=':
        if (inQuotes)
        {
          /* convert char to string when in quotes */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
        else
        {
          /* extract field */
          fieldTypeChar = 'c';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        break;

      default:
        /* check if punctuation, control, or non-printable */
        if (ispunct(curChar) || iscntrl(curChar) || !isprint(curChar)
#ifdef HAVE_ISBLANK
            || isblank(curChar)
#else
            || curChar == '\t'
#endif
        )
        {
          /* extract field */
          fieldTypeChar = 'c';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        else
        {
          /* convert char to string for other printable characters */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
        break;
      }
    }
    else if (curFieldType == FIELD_TYPE_NUM_HEX)
    {
      /******************************************************************
       ****************************** NUM HEX ***************************
       ******************************************************************/

#ifdef DEBUG
      count_num_hex++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=num_hex\n");
#endif

      /* XXX need to add code to handle hex numbers beginning with 0x */
      /* XXX need to add code to handle numbers beginning with 0 */
      if (FAST_ISXDIGIT(curChar))
      {
        /* Check case consistency for alphabetic hex digits */
        if (FAST_ISALPHA(curChar))
        {
          int currentCase = islower(curChar) ? 1 : 2;
          if (hexCase == 0)
          {
            /* First alphabetic hex digit, set the case */
            hexCase = currentCase;
          }
          else if (hexCase != currentCase)
          {
            /* Case mismatch - convert to string */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
            hexCase = 0; /* Reset for next field */
            break; /* Exit this case to process as string */
          }
        }
        runLen++;
        curLinePos++;
      }
      else if (FAST_ISALPHA(curChar) || (curChar == '@') ||
               ((inQuotes) && (curChar == ' ')) ||
               (curChar == '\\'))
      {
        /* convert field to string */
        curFieldType = FIELD_TYPE_STRING;
        runLen++;
        curLinePos++;
      }
      else if (curChar == ':')
      {

        if (runLen == 4)
        { /* check to see if this is the start of an IPv6
                              address */

          /* convert field to IPv4 */
          curFieldType = FIELD_TYPE_IP6;
          runLen++;
          startOfOctet = ++curLinePos;
          octet = 1;
          octetLen = 0;
        }
        else if (runLen == 2)
        { /* check to see if this is the start of a MAC
                                     address */

          /* convert field to MAC Address */
          curFieldType = FIELD_TYPE_MACADDR;
          runLen++;
          startOfOctet = ++curLinePos;
          octet = 1;
          octetLen = 0;
          macCase = hexCase; /* Inherit case from hex field */
          hexCase = 0; /* Reset hex case */
        }
        else
        {

          /* extract field */
          fieldTypeChar = 'x';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
      }
      else if (curChar == '-')
      {

        if (runLen == 2)
        { /* check to see if this is the start of a MAC address */

          /* convert field to MAC Address */
          curFieldType = FIELD_TYPE_MACADDR;
          runLen++;
          startOfOctet = ++curLinePos;
          octet = 1;
          octetLen = 0;
          macCase = hexCase; /* Inherit case from hex field */
          hexCase = 0; /* Reset hex case */
        }
        else
        {
          /* convert field to string */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }

#ifdef HAVE_ISBLANK
      }
      else if ((ispunct(curChar)) || (isblank(curChar)) ||
               (curChar == '/') || (curChar == '.') ||
               (iscntrl(curChar)) || !(isprint(curChar)))
      {
#else
      }
      else if ((ispunct(curChar)) || (curChar == ' ') ||
               (curChar == '\t') || (curChar == '/') ||
               (curChar == '.') || (iscntrl(curChar)) ||
               !(isprint(curChar)))
      {
#endif

        /* extract field */
        fieldTypeChar = 'x';
        curFieldType = FIELD_TYPE_EXTRACT;
        hexCase = 0; /* Reset hex case */
      }
    }

    else if (curFieldType == FIELD_TYPE_IP6)
    {
      /******************************************************************
       ****************************** IPv6 ******************************
       ******************************************************************/
#ifdef DEBUG
      count_ip6++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=IPv6\n");
#endif

      /* (xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx, xxxx:xxxx:xxxx:xxxx:: */

      /* XXX need to add code to handle numbers beginning with 0 */
      if (FAST_ISXDIGIT(curChar) && (octetLen < 4))
      {
        runLen++;
        curLinePos++;
        octetLen++;
      }
      else if ((octet < 8) && (curChar == ':'))
      {

        if ((octetLen > 0) && (octetLen <= 4))
        { /* is the octet valid */

          runLen++;
          startOfOctet = ++curLinePos;
          octet++;
          octetLen = 0;
        }
        else
        {

          /* not a valid ipv6 octet */
          /* XXX need to add rollback so we dont loose previous fields */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          runLen++;
          curLinePos++;
        }
      }
      else if (octet == 7)
      {

        if ((octetLen > 0) && (octetLen <= 4))
        { /* is the octet valid */

          /* extract field */
          fieldTypeChar = 'I';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        else
        {
          /* last octec is invalid */
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
        }
      }
      else
      {
        curFieldType = FIELD_TYPE_STRING;
      }
    }

    else if (curFieldType == FIELD_TYPE_NUM_FLOAT)
    {
      /******************************************************************
       ****************************** NUM FLOAT *************************
       ******************************************************************/

#ifdef DEBUG
      count_num_float++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=num_float\n");
#endif

      /****
       *
       * float
       *
       ****/

      /* XXX need to add code to handle numbers beginning with 0 */
      if (FAST_ISDIGIT(curChar))
      {
        runLen++;
        curLinePos++;
      }
      else if (FAST_ISALPHA(curChar) || (curChar == '@') ||
               ((inQuotes) && (curChar == ' ')) || (curChar == '\\') || (curChar == '.') || (curChar == ':') || (curChar == '-'))
      {
        /* convert field to string */
        curFieldType = FIELD_TYPE_STRING;
        runLen++;
        curLinePos++;
      }
      else
      {
        /* extract field */
        fieldTypeChar = 'f';
        curFieldType = FIELD_TYPE_EXTRACT;
      }
    }

    else if (curFieldType == FIELD_TYPE_BASE64)
    {
      /******************************************************************
       ****************************** BASE64 ****************************
       ******************************************************************/

#ifdef DEBUG
      count_base64++;
      if (config->debug >= 9)
        printf("DEBUG - STATE=base64 pos=%d blockpos=%d runlen=%d char='%c'\n",
               curLinePos, base64BlockPos, runLen, curChar);
#endif

      unsigned char b64val = base64_table[(unsigned char)curChar];

      if (b64val == 1)
      {
        /* Valid Base64 character - always continue as BASE64 regardless of saved type */
        /* BASE64 includes A-Z, a-z, 0-9, +, / so we should accept all of these */
        runLen++;
        curLinePos++;
        base64BlockPos = (base64BlockPos + 1) % 4;
        base64PaddingCount = 0; /* Reset padding if we see valid char */
      }
      else if (b64val == 2)
      {
        /* Padding character '=' */
        /* Allow padding if total length (including padding) will be >= 16 */
        if ((base64BlockPos == 2 || base64BlockPos == 3) && 
            (runLen + (4 - base64BlockPos) >= 16))
        {
          /* Valid padding position and will meet minimum length with padding */
          runLen++;
          curLinePos++;
          base64PaddingCount++;
          base64BlockPos = (base64BlockPos + 1) % 4;

          /* Check if we've completed a padded block */
          if (base64BlockPos == 0)
          {
            /* Extract as Base64 */
            fieldTypeChar = 'b';
            curFieldType = FIELD_TYPE_EXTRACT;
          }
        }
        else
        {
          /* Invalid padding - treat accumulated chars as STRING for multi-char sequences */
          if (runLen > 0)
          {
            /* Multi-character sequences that failed BASE64 should be STRING */
            if (runLen > 1)
            {
              fieldTypeChar = 's';
            }
            else
            {
              /* Single character - use saved type */
              if (savedFieldType == FIELD_TYPE_NUM_INT)
              {
                fieldTypeChar = 'd';
              }
              else if (savedFieldType == FIELD_TYPE_NUM_HEX)
              {
                fieldTypeChar = 'x';
              }
              else if (savedFieldType == FIELD_TYPE_CHAR)
              {
                fieldTypeChar = 'c';
              }
              else
              {
                fieldTypeChar = 's';
              }
            }
            curFieldType = FIELD_TYPE_EXTRACT;
          }
          else
          {
            /* Nothing to extract, reset to UNDEF */
            curFieldType = FIELD_TYPE_UNDEF;
          }
          base64BlockPos = 0;
          base64PaddingCount = 0;
        }
      }
      else if (b64val == 0)
      {
        /* Non-Base64 character encountered */
        if (runLen >= 16 && base64BlockPos == 0)
        {
          /* Valid Base64 ending on block boundary */
          fieldTypeChar = 'b';
          curFieldType = FIELD_TYPE_EXTRACT;
        }
        else
        {
          /* Not valid Base64 - decide based on length and saved type */
          if (savedFieldType == FIELD_TYPE_CHAR && runLen > 1)
          {
            /* Multi-character sequence starting with CHAR should become STRING */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          }
          else if (savedFieldType == FIELD_TYPE_NUM_INT && curChar == '.')
          {
            /* Check for IPv4 when NUM_INT encounters '.' - same logic as NUM_INT state */
            if ((runLen <= 3) && (atoi(line + startOfField) < 256))
            {
              /* Valid start of IPv4 address */
              curFieldType = FIELD_TYPE_IP4;
              runLen++;
              startOfOctet = ++curLinePos;
              octet = 1;
              octetLen = 0;
            }
            else
            {
              /* IPv4 test failed, transition to NUM_FLOAT like NUM_INT does */
              curFieldType = FIELD_TYPE_NUM_FLOAT;
              runLen++;
              curLinePos++;
            }
          }
          else if (savedFieldType == FIELD_TYPE_NUM_INT && curChar == '-')
          {
            /* Check for ISO date when NUM_INT encounters '-' - same logic as NUM_INT state */
            if ((runLen == 4) && (curLinePos + 12 <= lineLen))
            {
              /* look forward and see if this may be a date/time */
              /* XXX 2020-12-14 00:14:59.912 UTC */
              if (line[curLinePos + 3] == '-')
              {
                if (line[curLinePos + 6] == ' ')
                {
                  if (line[curLinePos + 9] == ':')
                  {
                    if (line[curLinePos + 12] == ':')
                    {
                      curFieldType = FIELD_TYPE_DT;
                      runLen++;
                      curLinePos++;
                    }
                    else
                    {
                      /* convert field to string */
                      curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                      runLen++;
                      curLinePos++;
                    }
                  }
                  else
                  {
                    /* convert field to string */
                    curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                    runLen++;
                    curLinePos++;
                  }
                }
                else
                {
                  /* convert field to string */
                  curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                  runLen++;
                  curLinePos++;
                }
              }
              else
              {
                /* convert field to string */
                curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
                runLen++;
                curLinePos++;
              }
            }
            else if (runLen == 2)
            {
              /* convert field to MAC */
              curFieldType = FIELD_TYPE_MACADDR;
              runLen++;
              startOfOctet = ++curLinePos;
              octet = 1;
              octetLen = 0;
            }
            else
            {
              /* extract field as number */
              fieldTypeChar = 'd';
              curFieldType = FIELD_TYPE_EXTRACT;
            }
          }
          else if (savedFieldType == FIELD_TYPE_NUM_INT && curChar == ':')
          {
            /* Check for MAC or IPv6 when NUM_INT encounters ':' - same logic as NUM_INT state */
            if (runLen == 2)
            {
              /* convert field to MAC */
              curFieldType = FIELD_TYPE_MACADDR;
              runLen++;
              startOfOctet = ++curLinePos;
              octet = 1;
              octetLen = 0;
            }
            else if (runLen == 4)
            {
              /* convert field to IPv6 */
              curFieldType = FIELD_TYPE_IP6;
              runLen++;
              startOfOctet = ++curLinePos;
              octet = 1;
              octetLen = 0;
            }
            else
            {
              /* extract field as number */
              fieldTypeChar = 'd';
              curFieldType = FIELD_TYPE_EXTRACT;
            }
          }
          else if (savedFieldType == FIELD_TYPE_NUM_INT && (curChar == '+' || curChar == '@' || curChar == '\\'))
          {
            /* For NUM_INT saved state, these characters should cause transition to STRING like in NUM_INT state */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
          }
          else if (savedFieldType == FIELD_TYPE_NUM_INT && FAST_ISALPHA(curChar))
          {
            /* convert field to string */
            curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
            runLen++;
            curLinePos++;
          }
          else if (savedFieldType == FIELD_TYPE_NUM_HEX && (curChar == ':' || curChar == '-'))
          {
            /* Check for MAC address when NUM_HEX encounters ':' or '-' */
            if (runLen == 2)
            {
              /* convert field to MAC */
              curFieldType = FIELD_TYPE_MACADDR;
              runLen++;
              startOfOctet = ++curLinePos;
              octet = 1;
              octetLen = 0;
              macCase = hexCase; /* Inherit case from hex field */
              hexCase = 0; /* Reset hex case */
            }
            else
            {
              /* extract field as hex */
              fieldTypeChar = 'x';
              curFieldType = FIELD_TYPE_EXTRACT;
              hexCase = 0; /* Reset hex case */
            }
          }
          else
          {
            /* Extract as appropriate type - multi-char sequences should be STRING */
            if (runLen > 1)
            {
              fieldTypeChar = 's';
            }
            else if (savedFieldType == FIELD_TYPE_NUM_INT)
            {
              fieldTypeChar = 'd';
            }
            else if (savedFieldType == FIELD_TYPE_NUM_HEX)
            {
              fieldTypeChar = 'x';
            }
            else if (savedFieldType == FIELD_TYPE_CHAR)
            {
              fieldTypeChar = 'c';
            }
            else
            {
              fieldTypeChar = 's';
            }
            curFieldType = FIELD_TYPE_EXTRACT;
          }
          base64BlockPos = 0;
          base64PaddingCount = 0;
        }
      }
    }

    else
    {
      /******************************************************************
       ****************************** UNDEF *****************************
       ******************************************************************/

#ifdef DEBUG
      if (config->debug >= 7)
        printf("*** UNDEF STATE: char='%c' at pos %d ***\n", curChar, curLinePos);
#endif

      /* Check if this might be the start of a syslog date - only at beginning of line */
      if ((curLinePos == 0) && (curChar == 'J' || curChar == 'F' || curChar == 'M' || curChar == 'A' ||
                                curChar == 'S' || curChar == 'O' || curChar == 'N' || curChar == 'D'))
      {
        if (isSyslogDate(line, curLinePos, lineLen))
        {
#ifdef DEBUG
          if (config->debug >= 3)
            printf("*** SYSLOG DATE DETECTED at pos %d: %.20s ***\n", curLinePos, line + curLinePos);
#endif
          curFieldType = FIELD_TYPE_DT_SYSLOG;
          runLen = 1;
          startOfField = curLinePos++;
        }
        else
        {
          /* Failed syslog test, switch to BASE64 state */
#ifdef DEBUG
          if (config->debug >= 8)
            printf("*** FAILED SYSLOG TEST, switching to BASE64 at pos %d: char='%c' ***\n", curLinePos, curChar);
#endif
          /* Determine the saved type based on character class */
          if (FAST_ISDIGIT(curChar))
          {
            savedFieldType = FIELD_TYPE_NUM_INT;
          }
          else if (FAST_ISXDIGIT(curChar) && !FAST_ISDIGIT(curChar))
          {
            savedFieldType = FIELD_TYPE_NUM_HEX;
            hexCase = islower(curChar) ? 1 : 2; /* Set case for hex */
          }
          else
          {
            savedFieldType = FIELD_TYPE_CHAR;
          }
          curFieldType = FIELD_TYPE_BASE64;
          base64BlockPos = 1;
          base64PaddingCount = 0;
          runLen = 1;
          startOfField = curLinePos++;
        }
      }
      else if (base64_table[(unsigned char)curChar] == 1)
      {
        /* Determine the saved type based on character class */
        if (FAST_ISDIGIT(curChar))
        {
          savedFieldType = FIELD_TYPE_NUM_INT;
        }
        else if (FAST_ISXDIGIT(curChar) && !FAST_ISDIGIT(curChar))
        {
          savedFieldType = FIELD_TYPE_NUM_HEX;
          hexCase = islower(curChar) ? 1 : 2; /* Set case for hex */
        }
        else
        {
          savedFieldType = FIELD_TYPE_CHAR;
        }

        curFieldType = FIELD_TYPE_BASE64;
        base64BlockPos = 1;
        base64PaddingCount = 0;
        runLen = 1;
        startOfField = curLinePos++;
      }
      else if (curChar == '*')
      {
        /* Treat * like other characters that can form strings */
        curFieldType = FIELD_TYPE_CHAR;
        runLen = 1;
        startOfField = curLinePos++;
      }
      else if (
               ((inQuotes) && (curChar == '/')) ||
               (curChar == '@') || (curChar == '%') ||
               (curChar == '$') || (curChar == '\\'))
      {
#ifdef DEBUG
        if (config->debug >= 8)
          printf("*** ALPHA CHECK PASSED for char='%c' at pos %d ***\n", curChar, curLinePos);
#endif

        curFieldType = FIELD_TYPE_CHAR;
        runLen = 1;
        startOfField = curLinePos++;
    }
    else if (curChar == '\"')
    {
      if (inQuotes)
      {
        /* something is really broke */
        runLen++;
        curLinePos++;
        inQuotes = FALSE;
      }
      else
      {
        if (!config->greedy)
        {
          if (templatePos > (MAX_FIELD_LEN - 2))
          {
            fprintf(stderr, "ERR - Template is too long\n");
            return (fieldPos - 1);
          }
          fields[0][templatePos++] = curChar;
          fields[0][templatePos] = '\0';
          curFieldType = FIELD_TYPE_STRING;
          macCase = 0; /* Reset mac case */
          inQuotes = TRUE;
          runLen = 0;
          startOfField = ++curLinePos;
        }
        else
        {
          /* printable but not alpha+num */
          if (templatePos > (MAX_FIELD_LEN - 2))
          {
            fprintf(stderr, "ERR - Template is too long\n");
            return (fieldPos - 1);
          }
          fields[0][templatePos++] = curChar;
          fields[0][templatePos] = '\0';
#ifdef DEBUG
          if (config->debug >= 10)
            printf("DEBUG - Updated template [%s]\n", fields[0]);
#endif
          runLen = 1;
          startOfField = curLinePos++;
        }
      }
    }
    else if ((iscntrl(curChar)) || !(isprint(curChar)))
    {
      /* not a valid log character, ignore it for now */
      curLinePos++;
#ifdef HAVE_ISBLANK
    }
    else if ((ispunct(curChar)) || (isblank(curChar)) ||
             (isprint(curChar)))
    {
#else
    }
    else if ((ispunct(curChar)) || (isprint(curChar)) ||
             (curChar == ' ') || (curChar == '\t'))
    {
#endif
      /* printable but not alpha+num */
      if (templatePos > (MAX_FIELD_LEN - 2))
      {
        fprintf(stderr, "ERR - Template is too long\n");
        return (fieldPos - 1);
      }
      fields[0][templatePos++] = curChar;
      fields[0][templatePos] = '\0';
#ifdef DEBUG
      if (config->debug >= 10)
        printf("DEBUG - Updated template [%s]\n", fields[0]);
#endif
      runLen = 1;
      startOfField = curLinePos++;
    }
    else
    {
      /* ignore it */
      curLinePos++;
    }
  }
  curChar = line[curLinePos];
}

/* Handle any incomplete field at end of line */
if (curFieldType == FIELD_TYPE_BASE64)
{
  if (runLen >= 16 && (base64BlockPos == 0 || (base64PaddingCount > 0 && base64BlockPos == 0)))
  {
    fieldTypeChar = 'b';
    curFieldType = FIELD_TYPE_EXTRACT;
  }
  else if (runLen >= 16)
  {
    /* Has minimum length but doesn't end on boundary - still could be valid BASE64 */
    fieldTypeChar = 'b';
    curFieldType = FIELD_TYPE_EXTRACT;
  }
  else
  {
    /* Not valid BASE64 - extract as appropriate type */
    if (runLen > 1)
    {
      /* Multi-character sequences should be STRING */
      fieldTypeChar = 's';
    }
    else if (savedFieldType == FIELD_TYPE_NUM_INT)
    {
      fieldTypeChar = 'd';
    }
    else if (savedFieldType == FIELD_TYPE_NUM_HEX)
    {
      fieldTypeChar = 'x';
    }
    else if (savedFieldType == FIELD_TYPE_CHAR)
    {
      fieldTypeChar = 'c';
    }
    else
    {
      fieldTypeChar = 's';
    }
    curFieldType = FIELD_TYPE_EXTRACT;
  }
}

/* Extract any pending field at end of line */
if (curFieldType == FIELD_TYPE_EXTRACT)
{
  if (fields[fieldPos] == NULL)
  {
    if ((fields[fieldPos] = (char *)XMALLOC(MAX_FIELD_LEN)) == NULL)
    {
      fprintf(stderr, "ERR - Unable to allocate memory for string\n");
      return (fieldPos - 1);
    }
  }
  fields[fieldPos][runLen + 1] = '\0';
  fields[fieldPos][0] = fieldTypeChar;
  XMEMCPY(fields[fieldPos] + 1, line + startOfField, runLen);

  /* update template */
  if (templatePos > (MAX_FIELD_LEN - 3))
  {
    fprintf(stderr, "ERR - Template is too long\n");
    return (fieldPos - 1);
  }
  fields[0][templatePos++] = '%';
  fields[0][templatePos++] = fieldTypeChar;
  fields[0][templatePos] = '\0';
  fieldPos++;
}

/* just in case the line was 0 length */
if (curLinePos == 0)
  return (0);

return (fieldPos);
}

/****
 *
 * return parsed field
 *
 ****/

int getParsedField(char *oBuf, int oBufLen, const unsigned int fieldNum)
{
  if ((fieldNum >= MAX_FIELD_POS) || (fields[fieldNum] == NULL))
  {
    fprintf(stderr, "ERR - Requested field does not exist [%d]\n", fieldNum);
    oBuf[0] = 0;
    return (FAILED);
  }
  XSTRNCPY(oBuf, fields[fieldNum], oBufLen);
  return (TRUE);
}

/****
 *
 * Get direct pointer to parsed field (no copy)
 *
 * DESCRIPTION:
 *   Returns a direct pointer to the parsed field data without copying.
 *   This is much more efficient than getParsedField when the caller
 *   only needs to read the field data and doesn't need a separate copy.
 *
 * PARAMETERS:
 *   fieldNum - Index of field to retrieve (0 = template)
 *
 * RETURNS:
 *   Pointer to field data on success
 *   NULL if field doesn't exist or is out of bounds
 *
 * SIDE EFFECTS:
 *   None - just returns existing pointer
 *
 * SECURITY FEATURES:
 *   - Validates field index against MAX_FIELD_POS
 *   - Returns const pointer to prevent modification
 *
 * PERFORMANCE:
 *   O(1) - Direct array access with no string copying
 *
 ****/
const char *getParsedFieldPtr(const unsigned int fieldNum)
{
  if ((fieldNum >= MAX_FIELD_POS) || (fields[fieldNum] == NULL))
  {
    return NULL;
  }
  return fields[fieldNum];
}

/****
 *
 * show debug state counts
 *
 ****/

void showCounts(void)
{
#ifdef DEBUG
  fprintf(stderr, "%-15lu STRING Count\n", count_string);
  fprintf(stderr, "%-15lu INT Count\n", count_num_int);
  fprintf(stderr, "%-15lu DATE Count\n", count_dt);
  fprintf(stderr, "%-15lu SYSLOG_DATE Count\n", count_dt_syslog);
  fprintf(stderr, "%-15lu EPOCH Count\n", count_dt_epoch);
  fprintf(stderr, "%-15lu ISO8601 Count\n", count_dt_iso8601);
  fprintf(stderr, "%-15lu APACHE Count\n", count_dt_apache);
  fprintf(stderr, "%-15lu EXTRACT Count\n", count_extract);
  fprintf(stderr, "%-15lu IPv4 Count\n", count_ip4);
  fprintf(stderr, "%-15lu MAC Count\n", count_mac);
  fprintf(stderr, "%-15lu CHAR Count\n", count_char);
  fprintf(stderr, "%-15lu HEX Count\n", count_num_hex);
  fprintf(stderr, "%-15lu IPv6 Count\n", count_ip6);
  fprintf(stderr, "%-15lu FLOAT Count\n", count_num_float);
  fprintf(stderr, "%-15lu URL Count\n", count_url);
  fprintf(stderr, "%-15lu BASE64 Count\n", count_base64);
#endif
}