/*****
 *
 * Description: String Interning System Headers
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

#ifndef STRING_INTERN_H
#define STRING_INTERN_H

/****
 *
 * includes
 *
 ****/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/****
 *
 * defines
 *
 ****/

#define STRING_POOL_SIZE (1024 * 1024)  /* 1MB string pool */
#define INTERN_HASH_INITIAL_SIZE 16384  /* Initial hash table size - 4x larger */
#define INTERN_HASH_MAX_LOAD_FACTOR 4   /* Resize when avg chain length > 4 */

/****
 *
 * typedefs & structs
 *
 ****/

/* Interned string reference */
typedef struct interned_string_s {
    uint32_t hash;                     /* Hash value for fast comparison */
    uint16_t length;                   /* String length */
    uint16_t ref_count;                /* Reference count */
    struct interned_string_s *next;    /* Next in hash chain */
    char data[0];                      /* Flexible array member for string data */
} interned_string_t;

/* String pool for memory management */
typedef struct string_pool_s {
    char *memory;                      /* Pool memory block */
    size_t size;                       /* Total pool size */
    size_t used;                       /* Used bytes */
    struct string_pool_s *next;        /* Next pool in chain */
} string_pool_t;

/* String interning system */
typedef struct string_intern_s {
    interned_string_t **hash_table;   /* Hash table for interned strings */
    uint32_t hash_size;               /* Hash table size */
    string_pool_t *pools;             /* Memory pools */
    uint32_t total_strings;           /* Total interned strings */
    size_t total_memory;              /* Total memory used */
} string_intern_t;

/****
 *
 * function prototypes
 *
 ****/

/* Initialize string interning system */
string_intern_t *initStringIntern(void);

/* Free string interning system */
void freeStringIntern(string_intern_t *intern);

/* Intern a string - returns pointer to interned copy */
const char *internString(string_intern_t *intern, const char *str);

/* Intern a string with known length */
const char *internStringLen(string_intern_t *intern, const char *str, size_t len);

/* Get interned string statistics */
void getInternStats(string_intern_t *intern, uint32_t *num_strings, size_t *memory_used);

/* Resize string intern hash table when load factor gets too high */
int resizeInternHashTable(string_intern_t *intern);

/* Fast string equality check for interned strings */
static inline int internedStringEqual(const char *a, const char *b) {
    return (a == b);  /* Pointer equality is sufficient for interned strings */
}

/* Global string interning system management */
string_intern_t *getGlobalIntern(void);
void cleanupGlobalIntern(void);

#endif /* STRING_INTERN_H */