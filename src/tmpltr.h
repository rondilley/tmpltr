/*****
 *
 * Description: Log Templater Headers
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

#ifndef TMPLTR_DOT_H
#define TMPLTR_DOT_H

/****
 *
 * defines
 *
 ****/

#define LINEBUF_SIZE 4096

/****
 *
 * includes
 *
 ****/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
#error something is messed up
#endif

#include "../include/common.h"
#include "hash.h"
#include "util.h"
#include "mem.h"
#include "parser.h"
#include "match.h"
#include "string_intern.h"

/****
 *
 * consts & enums
 *
 ****/

/****
 *
 * typedefs & structs
 *
 ****/

struct Fields_s
{
  uint16_t count;          /* Number of unique values stored */
  uint16_t capacity;       /* Capacity of values array */
  char **values;           /* Dynamic array of string pointers */
  uint8_t is_variable;     /* 1 if field is variable (too many values) */
  uint8_t tracking_enabled; /* 1 if still tracking new values */
  struct Fields_s *next;   /* Next field in linked list */
};

typedef struct
{
  char lBuf[LINEBUF_SIZE];
  size_t count;
  struct Fields_s *head;
} metaData_t;

/****
 *
 * function prototypes
 *
 ****/

int printTemplate(const struct hashRec_s *hashRec);
int processFile(const char *fName);
int showTemplates(void);
int loadTemplateFile(const char *fName);
char *clusterTemplate(char *template, metaData_t *md, char *oBuf, int bufSize);

/* Array-based field tracking functions */
void initField(struct Fields_s *field);
int trackFieldValue(struct Fields_s *field, const char *value);
void freeField(struct Fields_s *field);

#endif /* TMPLTR_DOT_H */
