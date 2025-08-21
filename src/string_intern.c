/*****
 *
 * Description: String Interning System Implementation
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
 * includes
 *
 ****/

#include "string_intern.h"
#include "mem.h"
#include "hash.h"

/****
 *
 * local variables
 *
 ****/

/* Global string interning system */
static string_intern_t *global_intern = NULL;

/****
 *
 * functions
 *
 ****/

/****
 *
 * Initialize string interning system
 *
 * DESCRIPTION:
 *   Creates and initializes a new string interning system with hash table
 *   and memory pools for efficient string storage and deduplication.
 *
 * RETURNS:
 *   Pointer to initialized string_intern_t structure
 *   NULL on allocation failure
 *
 * PERFORMANCE:
 *   O(1) - Simple allocation and initialization
 *
 ****/
string_intern_t *initStringIntern(void)
{
    string_intern_t *intern;
    uint32_t i;

    if ((intern = (string_intern_t *)XMALLOC(sizeof(string_intern_t))) == NULL) {
        return NULL;
    }

    intern->hash_size = INTERN_HASH_INITIAL_SIZE;
    if ((intern->hash_table = (interned_string_t **)XMALLOC(
             sizeof(interned_string_t *) * intern->hash_size)) == NULL) {
        XFREE(intern);
        return NULL;
    }

    /* Initialize hash table to NULL */
    for (i = 0; i < intern->hash_size; i++) {
        intern->hash_table[i] = NULL;
    }

    /* Create initial memory pool */
    if ((intern->pools = (string_pool_t *)XMALLOC(sizeof(string_pool_t))) == NULL) {
        XFREE(intern->hash_table);
        XFREE(intern);
        return NULL;
    }

    if ((intern->pools->memory = (char *)XMALLOC(STRING_POOL_SIZE)) == NULL) {
        XFREE(intern->pools);
        XFREE(intern->hash_table);
        XFREE(intern);
        return NULL;
    }

    intern->pools->size = STRING_POOL_SIZE;
    intern->pools->used = 0;
    intern->pools->next = NULL;
    intern->total_strings = 0;
    intern->total_memory = STRING_POOL_SIZE;

    return intern;
}

/****
 *
 * Free string interning system
 *
 * DESCRIPTION:
 *   Frees all memory associated with the string interning system,
 *   including hash table and memory pools.
 *
 * PARAMETERS:
 *   intern - String interning system to free
 *
 * SIDE EFFECTS:
 *   All interned string pointers become invalid
 *
 ****/
void freeStringIntern(string_intern_t *intern)
{
    string_pool_t *pool, *next_pool;

    if (!intern) return;

    /* Free all memory pools */
    pool = intern->pools;
    while (pool) {
        next_pool = pool->next;
        XFREE(pool->memory);
        XFREE(pool);
        pool = next_pool;
    }

    /* Free hash table (strings are freed with pools) */
    XFREE(intern->hash_table);
    XFREE(intern);
}

/****
 *
 * Allocate space from string pool
 *
 * DESCRIPTION:
 *   Allocates space from the current string pool, creating a new pool
 *   if necessary. Uses simple bump-pointer allocation for efficiency.
 *
 * PARAMETERS:
 *   intern - String interning system
 *   size - Number of bytes to allocate
 *
 * RETURNS:
 *   Pointer to allocated memory
 *   NULL on allocation failure
 *
 ****/
static void *allocFromPool(string_intern_t *intern, size_t size)
{
    string_pool_t *pool = intern->pools;
    void *result;

    /* Align to 8-byte boundary for performance */
    size = (size + 7) & ~7;

    /* Find pool with enough space */
    while (pool && pool->used + size > pool->size) {
        if (!pool->next) {
            /* Create new pool */
            if ((pool->next = (string_pool_t *)XMALLOC(sizeof(string_pool_t))) == NULL) {
                return NULL;
            }
            if ((pool->next->memory = (char *)XMALLOC(STRING_POOL_SIZE)) == NULL) {
                XFREE(pool->next);
                pool->next = NULL;
                return NULL;
            }
            pool->next->size = STRING_POOL_SIZE;
            pool->next->used = 0;
            pool->next->next = NULL;
            intern->total_memory += STRING_POOL_SIZE;
        }
        pool = pool->next;
    }

    /* Allocate from current pool */
    result = pool->memory + pool->used;
    pool->used += size;
    return result;
}

/****
 *
 * Intern a string with known length
 *
 * DESCRIPTION:
 *   Interns a string with known length for optimal performance.
 *   Uses FNV-1a hash for fast comparison and efficient distribution.
 *
 * PARAMETERS:
 *   intern - String interning system
 *   str - String to intern
 *   len - Length of string (excluding null terminator)
 *
 * RETURNS:
 *   Pointer to interned string (always null-terminated)
 *   NULL on allocation failure
 *
 * PERFORMANCE:
 *   Average O(1) lookup, O(n) worst case for hash collisions
 *   Memory usage: One copy per unique string
 *
 ****/
const char *internStringLen(string_intern_t *intern, const char *str, size_t len)
{
    uint32_t hash;
    uint32_t bucket;
    interned_string_t *entry;
    size_t total_size;

    if (!intern || !str) return NULL;

    /* Compute FNV-1a hash */
    hash = fnv1aHash(str, len);
    bucket = hash % intern->hash_size;

    /* Search for existing string */
    entry = intern->hash_table[bucket];
    while (entry) {
        if (entry->hash == hash && 
            entry->length == len &&
            memcmp(entry->data, str, len) == 0) {
            /* Found existing string */
            entry->ref_count++;
            return entry->data;
        }
        entry = entry->next;
    }

    /* Create new interned string */
    total_size = sizeof(interned_string_t) + len + 1; /* +1 for null terminator */
    if ((entry = (interned_string_t *)allocFromPool(intern, total_size)) == NULL) {
        return NULL;
    }

    /* Initialize entry */
    entry->hash = hash;
    entry->length = len;
    entry->ref_count = 1;
    entry->next = intern->hash_table[bucket];
    memcpy(entry->data, str, len);
    entry->data[len] = '\0'; /* Ensure null termination */

    /* Add to hash table */
    intern->hash_table[bucket] = entry;
    intern->total_strings++;
    
    /* Check if we need to resize hash table */
    if (intern->total_strings > intern->hash_size * INTERN_HASH_MAX_LOAD_FACTOR) {
        resizeInternHashTable(intern);
    }

    return entry->data;
}

/****
 *
 * Intern a null-terminated string
 *
 * DESCRIPTION:
 *   Convenience wrapper for internStringLen that computes string length.
 *
 * PARAMETERS:
 *   intern - String interning system
 *   str - Null-terminated string to intern
 *
 * RETURNS:
 *   Pointer to interned string
 *   NULL on allocation failure
 *
 ****/
const char *internString(string_intern_t *intern, const char *str)
{
    if (!str) return NULL;
    return internStringLen(intern, str, strlen(str));
}

/****
 *
 * Resize string intern hash table
 *
 * DESCRIPTION:
 *   Doubles the hash table size and rehashes all entries when the load factor
 *   becomes too high. This maintains O(1) average lookup performance.
 *
 * PARAMETERS:
 *   intern - String interning system
 *
 * RETURNS:
 *   1 on success, 0 on failure
 *
 ****/
int resizeInternHashTable(string_intern_t *intern)
{
    interned_string_t **old_table, **new_table;
    uint32_t old_size, new_size, i;
    interned_string_t *entry, *next;
    
    if (!intern) return 0;
    
    old_table = intern->hash_table;
    old_size = intern->hash_size;
    new_size = old_size * 2;
    
    /* Prevent excessive memory usage - cap at 1M buckets */
    if (new_size > 1048576) return 0;
    
    /* Allocate new hash table */
    new_table = (interned_string_t **)XMALLOC(sizeof(interned_string_t *) * new_size);
    if (!new_table) return 0;
    
    /* Initialize new table */
    for (i = 0; i < new_size; i++) {
        new_table[i] = NULL;
    }
    
    /* Rehash all entries */
    for (i = 0; i < old_size; i++) {
        entry = old_table[i];
        while (entry) {
            next = entry->next;
            
            /* Calculate new bucket */
            uint32_t new_bucket = entry->hash % new_size;
            
            /* Insert into new table */
            entry->next = new_table[new_bucket];
            new_table[new_bucket] = entry;
            
            entry = next;
        }
    }
    
    /* Update intern structure */
    intern->hash_table = new_table;
    intern->hash_size = new_size;
    
    /* Free old table */
    XFREE(old_table);
    
    return 1;
}

/****
 *
 * Get string interning statistics
 *
 * DESCRIPTION:
 *   Returns statistics about the string interning system for monitoring
 *   and debugging purposes.
 *
 * PARAMETERS:
 *   intern - String interning system
 *   num_strings - Output: number of unique interned strings
 *   memory_used - Output: total memory used by pools
 *
 ****/
void getInternStats(string_intern_t *intern, uint32_t *num_strings, size_t *memory_used)
{
    if (!intern) {
        if (num_strings) *num_strings = 0;
        if (memory_used) *memory_used = 0;
        return;
    }

    if (num_strings) *num_strings = intern->total_strings;
    if (memory_used) *memory_used = intern->total_memory;
}

/****
 *
 * Get or initialize global string interning system
 *
 * DESCRIPTION:
 *   Returns the global string interning system, creating it if necessary.
 *   This provides a convenient default interning system for the application.
 *
 * RETURNS:
 *   Pointer to global string interning system
 *   NULL on allocation failure
 *
 ****/
string_intern_t *getGlobalIntern(void)
{
    if (!global_intern) {
        global_intern = initStringIntern();
    }
    return global_intern;
}

/****
 *
 * Cleanup global string interning system
 *
 * DESCRIPTION:
 *   Frees the global string interning system. Should be called during
 *   application shutdown to prevent memory leaks.
 *
 ****/
void cleanupGlobalIntern(void)
{
    if (global_intern) {
        freeStringIntern(global_intern);
        global_intern = NULL;
    }
}