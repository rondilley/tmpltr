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

/* Storage types for field values */
#define FIELD_STORAGE_INLINE  0  /* Using inline array (count <= 8) */
#define FIELD_STORAGE_DYNAMIC 1  /* Using dynamic array (8 < count <= 32) */
#define FIELD_STORAGE_HASHSET 2  /* Using hash set (count > 32) */

/* Inline array size - optimized for cache line */
#define FIELD_INLINE_SIZE 8

/* Threshold to switch from dynamic array to hash set */
#define FIELD_HASHSET_THRESHOLD 32

/* Simple hash set for field values */
typedef struct {
  const char **buckets;    /* Array of interned string pointers */
  uint16_t capacity;        /* Power of 2 for fast modulo */
  uint16_t count;          /* Number of entries */
  uint16_t max_probe;      /* Maximum probe distance seen */
} field_hashset_t;

struct Fields_s
{
  uint16_t count;          /* Number of unique values stored */
  uint8_t storage_type;    /* INLINE, DYNAMIC, or HASHSET */
  uint8_t is_variable;     /* 1 if field is variable (too many values) */
  uint8_t tracking_enabled; /* 1 if still tracking new values */
  
  union {
    /* For small counts (<=8): inline array */
    const char *inline_values[FIELD_INLINE_SIZE];
    
    /* For medium counts (8-32): dynamic array */
    struct {
      const char **values;
      uint16_t capacity;
    } dynamic;
    
    /* For large counts (>32): hash set */
    field_hashset_t *hashset;
  } storage;
  
  struct Fields_s *next;   /* Next field in linked list */
};

typedef struct
{
  char lBuf[LINEBUF_SIZE];
  size_t count;
  struct Fields_s *head;
  uint8_t all_fields_stopped_tracking; /* 1 if all fields have stopped tracking */
  uint8_t template_complete; /* 1 if this template has enough field samples */
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

/* Hybrid field tracking functions */
void initField(struct Fields_s *field);
int trackFieldValue(struct Fields_s *field, const char *value);
void freeField(struct Fields_s *field);

/* Hash set helper functions */
field_hashset_t *field_hashset_create(uint16_t initial_capacity);
void field_hashset_destroy(field_hashset_t *hs);
int field_hashset_contains_or_add(field_hashset_t *hs, const char *interned_str);
void field_hashset_resize(field_hashset_t *hs);

#endif /* TMPLTR_DOT_H */
