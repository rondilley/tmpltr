/*****
 *
 * Description: Log Templater Functions
 *
 * Copyright (c) 2008-2023, Ron Dilley
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
#include "parser_interface.h"

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

/* parser interface */
PRIVATE ParserInterface *current_parser = NULL;

/****
 *
 * external variables
 *
 ****/

extern Config_t *config;
extern int quit;
extern int reload;

/****
 * secure file open with symlink protection
 ****/
PRIVATE FILE *secure_fopen(const char *path, const char *mode)
{
  int flags = 0;
  int fd;
  FILE *fp;
  
  if (!path || !mode) {
    return NULL;
  }
  
  /* Determine flags based on mode */
  if (strchr(mode, 'r') && !strchr(mode, '+')) {
    flags = O_RDONLY | O_NOFOLLOW;
  } else if (strchr(mode, 'w')) {
    flags = O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW;
  } else if (strchr(mode, 'a')) {
    flags = O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW;
  } else if (strchr(mode, '+')) {
    if (strchr(mode, 'r')) {
      flags = O_RDWR | O_NOFOLLOW;
    } else if (strchr(mode, 'w')) {
      flags = O_RDWR | O_CREAT | O_TRUNC | O_NOFOLLOW;
    } else if (strchr(mode, 'a')) {
      flags = O_RDWR | O_CREAT | O_APPEND | O_NOFOLLOW;
    }
  } else {
    fprintf(stderr, "ERR - Invalid file mode: %s\n", mode);
    return NULL;
  }
  
  /* Open file with O_NOFOLLOW to prevent symlink attacks */
  fd = open(path, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd == -1) {
    if (errno == ELOOP) {
      fprintf(stderr, "ERR - Symbolic link detected, access denied: %s\n", path);
    }
    return NULL;
  }
  
  /* Convert file descriptor to FILE* */
  fp = fdopen(fd, mode);
  if (fp == NULL) {
    close(fd);
    return NULL;
  }
  
  return fp;
}

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

char *clusterTemplate(char *template, metaData_t *md, char *oBuf, int bufSize)
{
  int i, rPos = 0, wPos = 0, done;
  struct Fields_s *curFieldPtr = md->head;

  /* Ensure we have space for at least null terminator */
  if (bufSize <= 0) {
    return NULL;
  }

  /* Initialize buffer */
  oBuf[0] = '\0';

  /* loop through the field argument trees */
  while ((curFieldPtr != NULL) && (wPos < bufSize - 1))
  {
    /* find next variable */
    done = FALSE;
    while (!done && (wPos < bufSize - 1))
    {
      /* Copy template characters until % or end, with bounds checking */
      while ((template[rPos] != 0) && (template[rPos] != '%') && (wPos < bufSize - 1))
        oBuf[wPos++] = template[rPos++];

      /* Check for buffer full or template end */
      if ((wPos >= bufSize - 1) || (template[rPos] == 0))
      {
        done = TRUE;
      }
      else if (template[rPos] == '%')
      {
        /* Check for valid format specifier and buffer space */
        if ((template[rPos + 1] == 'd') ||
            (template[rPos + 1] == 'f') ||
            (template[rPos + 1] == 's') ||
            (template[rPos + 1] == 'c') ||
            (template[rPos + 1] == 't') ||
            (template[rPos + 1] == 'x') ||
            (template[rPos + 1] == 'm') ||
            (template[rPos + 1] == 'i') ||
            (template[rPos + 1] == 'I'))
        {
          if (curFieldPtr->count == 1 && curFieldPtr->values && curFieldPtr->values[0])
          {
            /* move past the place holder */
            rPos += 2;
            
            /* Copy field value with bounds checking - skip type prefix */
            const char *fieldValue = curFieldPtr->values[0] + 1;
            for (i = 0; fieldValue[i] != 0 && wPos < bufSize - 1; i++)
            {
              if (isprint(fieldValue[i]))
              {
                /* Single character - check space */
                if (wPos < bufSize - 1) {
                  oBuf[wPos++] = fieldValue[i];
                } else {
                  break; /* Buffer full */
                }
              }
              else
              {
                /* Hex escape sequence needs 4 characters: \x##  */
                if (wPos <= bufSize - 5) {
                  int written = snprintf(oBuf + wPos, bufSize - wPos, "\\x%02x", 
                                       (unsigned char)fieldValue[i]);
                  if (written > 0 && written < bufSize - wPos) {
                    wPos += written;
                  } else {
                    break; /* Buffer full or error */
                  }
                } else {
                  break; /* Not enough space for hex sequence */
                }
              }
            }
          }
          else
          {
            /* Copy the % and format character if space available */
            if (wPos < bufSize - 2) {
              oBuf[wPos++] = template[rPos++];
              oBuf[wPos++] = template[rPos++];
            } else {
              done = TRUE; /* Buffer full */
            }
          }
          done = TRUE;
        }
        else
        {
          /* Invalid format specifier, copy the % character */
          if (wPos < bufSize - 1) {
            oBuf[wPos++] = template[rPos++];
          } else {
            done = TRUE; /* Buffer full */
          }
        }
      }
      else
      {
        /* Copy single character with bounds check */
        if (wPos < bufSize - 1) {
          oBuf[wPos++] = template[rPos++];
        } else {
          done = TRUE; /* Buffer full */
        }
      }
    }
    curFieldPtr = curFieldPtr->next;
  }

  /* copy the rest of the characters with bounds checking */
  while ((template[rPos] != 0) && (wPos < bufSize - 1))
    oBuf[wPos++] = template[rPos++];
  
  /* Ensure null termination */
  oBuf[wPos] = '\0';

  return (oBuf);
}

/****
 *
 * print all template records in hash
 *
 ****/

int printTemplate(const struct hashRec_s *hashRec)
{
  metaData_t *tmpMd;
  struct Fields_s *curFieldPtr, *tmpFieldPtr;
  char oBuf[MAX_FIELD_LEN];

#ifdef DEBUG
  if (config->debug >= 3)
    printf("DEBUG - Searching for [%s]\n", hashRec->keyString);
#endif

  if (hashRec->data != NULL)
  {
    tmpMd = (metaData_t *)hashRec->data;
    /* save template if -w was used */
    if (config->outFile_st != NULL)
      fprintf(config->outFile_st, "%s\n", hashRec->keyString);

    if (config->cluster)
    {
      if (tmpMd->count > 1)
        printf("%12lu %s||%s", tmpMd->count, clusterTemplate(hashRec->keyString, tmpMd, oBuf, sizeof(oBuf)), tmpMd->lBuf);
      else
        printf("%12lu %s\n", tmpMd->count, clusterTemplate(hashRec->keyString, tmpMd, oBuf, sizeof(oBuf)));
    }
    else
      printf("%12lu %s||%s", tmpMd->count, hashRec->keyString, tmpMd->lBuf);

    /* cleanup after clustering */
    if (config->cluster)
    {
      if (tmpMd != NULL)
        curFieldPtr = tmpMd->head;
      else
        curFieldPtr = NULL;

      /* free per field records and arrays */
      while (curFieldPtr != NULL)
      {
        tmpFieldPtr = curFieldPtr;
        curFieldPtr = curFieldPtr->next;
        freeField(tmpFieldPtr);
        XFREE(tmpFieldPtr);
      }
    }

    XFREE(tmpMd);
  }

  /* can use this later to interrupt traversing the hash */
  if (quit)
    return (TRUE);
  return (FALSE);
}

/****
 *
 * process file
 *
 ****/

int processFile(const char *fName)
{
  FILE *inFile = NULL;
  char inBuf[65536];  /* 64KB buffer for better I/O performance */
  char oBuf[8192];
  PRIVATE int i, ret;
  unsigned int lineCount = 0;
#ifdef DEBUG
  unsigned int lineLen = 0, minLineLen = sizeof(inBuf), maxLineLen = 0, totLineLen = 0;
  unsigned int argCount = 0, totArgCount = 0, minArgCount = MAX_FIELD_POS, maxArgCount = 0;
#endif
  struct hashRec_s *tmpRec;
  metaData_t *tmpMd;
  struct Fields_s **curFieldPtr;

  /* initialize the hash if we need to */
  if (templateHash == NULL)
    templateHash = initHash(52);

  /* get and initialize the selected parser */
  current_parser = getParser((ParserType)config->parser_type);
  if (current_parser == NULL) {
    fprintf(stderr, "ERR - Unable to get parser interface\n");
    return (EXIT_FAILURE);
  }
  
  if (config->debug >= 2) {
    fprintf(stderr, "DEBUG - Using parser: %s\n", current_parser->name);
  }
  
  current_parser->init();

  fprintf(stderr, "Opening [%s] for read\n", fName);
  if (strcmp(fName, "-") == 0)
  {
    inFile = stdin;
  }
  else
  {
    if ((inFile = secure_fopen(fName, "r")) == NULL)
    {
      fprintf(stderr, "ERR - Unable to open file [%s] %d (%s)\n", fName, errno, strerror(errno));
      return (EXIT_FAILURE);
    }
  }

  while (fgets(inBuf, sizeof(inBuf), inFile) != NULL && !quit)
  {
    if (reload == TRUE)
    {
      fprintf(stderr, "Processed %d lines/min\n", lineCount);
#ifdef DEBUG
      if (config->debug)
      {
        fprintf(stderr, "Line length: min=%d, max=%d, avg=%2.0f\n", minLineLen, maxLineLen, (float)totLineLen / (float)lineCount);

        if (config->cluster)
        {
          fprintf(stderr, "Stored %d args/min\n", argCount);
          fprintf(stderr, "Arg count: min/%d, max/%d, avg/%2.0f\n", minArgCount, maxArgCount, (float)totArgCount / (float)lineCount);
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
    if (config->debug)
    {
      lineLen = strlen(inBuf);
      totLineLen += lineLen;
      if (lineLen < minLineLen)
        minLineLen = lineLen;
      else if (lineLen > maxLineLen)
        maxLineLen = lineLen;
    }
#endif

    if (config->debug >= 3)
      printf("DEBUG - Before [%s]", inBuf);

    if ((ret = current_parser->parseLine(inBuf)) > 0)
    {

#ifdef DEBUG
      if (config->debug)
      {
        /* save arg count */
        totArgCount += ret;
        if (ret < minArgCount)
          minArgCount = ret;
        else if (ret > maxArgCount)
          maxArgCount = ret;
      }
#endif

      /* the first field is the generated template */
      current_parser->getParsedField(oBuf, sizeof(oBuf), 0);

      if (config->match)
      {
        if (templateMatches(oBuf))
          printf("%s", inBuf);
      }
      else
      {
        /* load it into the hash */
        if ((tmpRec = getHashRecord(templateHash, oBuf, strlen(oBuf) + 1)) == NULL)
        { /* new template */

#ifdef DEBUG
          if (config->debug >= 3)
            printf("%s||%s", oBuf, inBuf);
#endif

          /* store line metadata */
          tmpMd = (metaData_t *)XMALLOC(sizeof(metaData_t));
          XMEMSET(tmpMd, 0, sizeof(metaData_t));
          tmpMd->count = 1;
          XSTRNCPY(tmpMd->lBuf, inBuf, LINEBUF_SIZE);

          /* stuff the new record into the hash before processing fields */
          if ((tmpRec = addUniqueHashRec(templateHash, oBuf, strlen(oBuf) + 1, tmpMd)) == NULL)
          {
            fprintf(stderr, "ERR - Unable to add hash record\n");
          }
          else
          {
            /* process arguments if clustering is enabled */
            if (config->cluster)
            {
              curFieldPtr = &tmpMd->head;
              for (i = 1; i < ret; i++)
              {
                current_parser->getParsedField(inBuf, sizeof(inBuf), i);

                /* XXX removing chain stubs and moving to a separate tool */
#ifdef DEBUG
                if (config->debug >= 4)
                  printf("DEBUG - Storing argument [%s]\n", inBuf);
#endif

                if (*curFieldPtr == NULL)
                {
                  *curFieldPtr = (struct Fields_s *)XMALLOC(sizeof(struct Fields_s));
                  initField(*curFieldPtr);
                }
                trackFieldValue(*curFieldPtr, inBuf);
#ifdef DEBUG
                if (config->debug)
                  argCount++;
#endif
                curFieldPtr = &(*curFieldPtr)->next;
              }
            }
          }
          /* grow the hash if it is too large */
          if (templateHash->totalRecords > templateHash->size)
            templateHash = dyGrowHash(templateHash);
        }
        else
        {

          if (tmpRec->data != NULL)
          {
            tmpMd = (metaData_t *)tmpRec->data;
            tmpMd->count++;

#ifdef DEBUG
            if (config->debug >= 3)
              printf("DEBUG - Updating existing template\n");
#endif

            /* update the argument info if clustering is enabled */
            if (config->cluster)
            {
              curFieldPtr = &tmpMd->head;
              for (i = 1; i < ret; i++)
              {
                current_parser->getParsedField(inBuf, sizeof(inBuf), i);

#ifdef DEBUG
                if (config->debug >= 4)
                  printf("DEBUG - Processing argument [%s]\n", inBuf);
#endif

                if (*curFieldPtr == NULL)
                {
                  *curFieldPtr = (struct Fields_s *)XMALLOC(sizeof(struct Fields_s));
                  initField(*curFieldPtr);
                }

                /* Track field value using efficient array-based approach */
                if (trackFieldValue(*curFieldPtr, inBuf) == 1)
                {
#ifdef DEBUG
                  if (config->debug)
                    argCount++;
#endif
                }
                curFieldPtr = &(*curFieldPtr)->next;
              }
            }
          }
        }
      }
      lineCount++;
    }
  }

#ifdef DEBUG
  if (config->debug)
  {
    fprintf(stderr, "Line length: min=%d, max=%d, avg=%2.0f\n", minLineLen, maxLineLen, (float)totLineLen / (float)lineCount);

    if (config->cluster)
    {
      fprintf(stderr, "Stored %d args/min\n", argCount);
      fprintf(stderr, "Arg count: min/%d, max/%d, avg/%2.0f\n", minArgCount, maxArgCount, (float)totArgCount / (float)lineCount);
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

  if (inFile != stdin)
    fclose(inFile);

  if (current_parser != NULL) {
    current_parser->deinit();
  }

  return (EXIT_SUCCESS);
}

/****
 *
 * print templates
 *
 ****/

int showTemplates(void)
{

#ifdef DEBUG
  if (config->debug >= 1)
    printf("DEBUG - Finished processing file, printing\n");
#endif

  if (templateHash != NULL)
  {
    /* dump the template data */
    if (traverseHash(templateHash, printTemplate) == TRUE)
    {
      freeHash(templateHash);
      return (EXIT_SUCCESS);
    }
    freeHash(templateHash);
  }

  return (EXIT_FAILURE);
}

/****
 *
 * load templates
 *
 ****/

int loadTemplateFile(const char *fName)
{
  FILE *inFile;
  char inBuf[65536];  /* 64KB buffer for better I/O performance */
  size_t count = 0;
  int lLen, i;

  /* init the hash if we need to */
  if (templateHash == NULL)
    templateHash = initHash(52);

#ifdef DEBUG
  if (config->debug >= 1)
    printf("DEBUG - Loading template file [%s]\n", fName);
#endif

  if ((inFile = secure_fopen(fName, "r")) == NULL)
  {
    fprintf(stderr, "ERR - Unable to open template file [%s]\n", fName);
    return (FAILED);
  }

  while (fgets(inBuf, sizeof(inBuf), inFile) != NULL)
  {
    if (inBuf[0] != '#')
    {
      /* strip of <CR> or <LF> */
      lLen = strlen(inBuf);
      for (i = 0; i < lLen; i++)
      {
        if (inBuf[i] == '\n' || inBuf[i] == '\r')
        {
          inBuf[i] = '\0';
          i = lLen;
        }
      }

#ifdef DEBUG
      if (config->debug >= 3)
        printf("DEBUG - Loading template [%s]\n", inBuf);
#endif

      count++;
      addUniqueHashRec(templateHash, inBuf, strlen(inBuf) + 1, NULL);
    }
  }

  fclose(inFile);

#ifdef DEBUG
  if (config->debug >= 1)
    printf("DEBUG - Loaded [%lu] templates from ignore file\n", count);
#endif

  return (TRUE);
}

/****
 *
 * Initialize a field structure
 *
 * DESCRIPTION:
 *   Initializes a Fields_s structure for array-based value tracking.
 *   Sets up initial state with tracking enabled.
 *
 * PARAMETERS:
 *   field - Pointer to field structure to initialize
 *
 * SIDE EFFECTS:
 *   Zeros the field structure and enables tracking
 *
 ****/
void initField(struct Fields_s *field)
{
  if (field == NULL)
    return;
    
  field->count = 0;
  field->capacity = 0;
  field->values = NULL;
  field->is_variable = 0;
  field->tracking_enabled = 1;
  field->next = NULL;
}

/****
 *
 * Track a field value using array-based approach
 *
 * DESCRIPTION:
 *   Tracks unique field values using a dynamic array instead of binary trees.
 *   Provides O(n) search but with excellent cache locality for small n.
 *   Automatically stops tracking when field becomes too variable.
 *
 * PARAMETERS:
 *   field - Pointer to field structure
 *   value - String value to track
 *
 * RETURNS:
 *   1 if value was added (new unique value)
 *   0 if value already exists or tracking disabled
 *   -1 on error
 *
 * PERFORMANCE:
 *   O(n) linear search but 3-6x faster than binary trees for small datasets
 *   due to cache locality and simplified operations
 *
 ****/
int trackFieldValue(struct Fields_s *field, const char *value)
{
  uint16_t i;
  char **newValues;
  uint16_t newCapacity;
  const char *internedValue;
  string_intern_t *intern;
  
  if (field == NULL || value == NULL || !field->tracking_enabled)
    return 0;
  
  /* Get global string interning system */
  intern = getGlobalIntern();
  if (!intern)
    return -1;
  
  /* Intern the value for memory efficiency */
  internedValue = internString(intern, value);
  if (!internedValue)
    return -1;
    
  /* Check if we've seen this value before using pointer equality for interned strings */
  for (i = 0; i < field->count; i++)
  {
    if (field->values[i] != NULL && field->values[i] == internedValue)
      return 0; /* Already exists - pointer equality check is O(1) */
  }
  
  /* Stop tracking if we exceed clusterDepth (too many unique values) */
  if (field->count >= config->clusterDepth)
  {
    field->is_variable = 1;
    field->tracking_enabled = 0;
    return 0;
  }
  
  /* Need to grow the array? */
  if (field->count >= field->capacity)
  {
    newCapacity = field->capacity == 0 ? 4 : field->capacity * 2;
    newValues = (char **)XREALLOC(field->values, sizeof(char *) * newCapacity);
    if (newValues == NULL)
      return -1;
      
    field->values = newValues;
    field->capacity = newCapacity;
  }
  
  /* Add the interned value (no need to copy, just store pointer) */
  field->values[field->count] = (char *)internedValue;
  field->count++;
  return 1; /* New value added */
}

/****
 *
 * Free field structure and all stored values
 *
 * DESCRIPTION:
 *   Frees all memory associated with a field structure including
 *   the dynamic array and all stored string values.
 *
 * PARAMETERS:
 *   field - Pointer to field structure to free
 *
 * SIDE EFFECTS:
 *   Frees all allocated memory, field structure becomes invalid
 *
 ****/
void freeField(struct Fields_s *field)
{
  if (field == NULL)
    return;
    
  /* Free the values array (individual strings are interned, don't free them) */
  if (field->values != NULL)
  {
    XFREE(field->values);
    field->values = NULL;
  }
  
  field->count = 0;
  field->capacity = 0;
  field->is_variable = 0;
  field->tracking_enabled = 0;
}
