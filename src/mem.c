/****
 *
 * Memory functions
 *
 * Copyright (c) 2006-2025, Ron Dilley
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

#include "mem.h"

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

extern int quit;

/****
 *
 * global variables
 *
 ****/

#ifdef MEM_DEBUG
PRIVATE struct Mem_s *head;
PRIVATE struct Mem_s *tail;
#endif

/****
 *
 * functions
 *
 ****/

/****
 *
 * Copy argv into a newly malloced buffer.  Arguments are concatenated
 * with spaces in between each argument.
 *
 ****/

/****
 *
 * Safely concatenate command line arguments with overflow protection
 *
 * DESCRIPTION:
 *   Concatenates all command line arguments into a single string with
 *   spaces between arguments. Includes comprehensive overflow protection
 *   and bounds checking to prevent buffer overflow vulnerabilities.
 *
 * PARAMETERS:
 *   argv - Array of argument strings (NULL-terminated)
 *
 * RETURNS:
 *   Pointer to allocated string containing concatenated arguments
 *   NULL if memory allocation fails or overflow would occur
 *
 * SIDE EFFECTS:
 *   Allocates memory for result string
 *
 * SECURITY FEATURES:
 *   - Checks for integer overflow in length calculations
 *   - Validates total length against SIZE_MAX
 *   - Bounds checking during string copying
 *
 * MEMORY MANAGEMENT:
 *   Caller is responsible for freeing returned string
 *
 ****/

PUBLIC char *copy_argv(char *argv[])
{
  PRIVATE char **arg;
  PRIVATE char *buf;
  PRIVATE size_t total_length = 0;

  for (arg = argv; *arg != NULL; arg++)
  {
    size_t arg_len = strlen(*arg);
    /* Check for integer overflow */
    if (total_length > SIZE_MAX - arg_len - 1)
    {
      fprintf(stderr, "ERR - Integer overflow in copy_argv: argument too long\n");
      return NULL;
    }
    total_length += (arg_len + 1); /* length of arg plus space */
  }

  if (total_length == 0)
    return NULL;

  /* Check for overflow when adding room for null */
  if (total_length > SIZE_MAX - 1)
  {
    fprintf(stderr, "ERR - Integer overflow in copy_argv (null terminator)\n");
    return NULL;
  }
  total_length++; /* add room for a null */

  buf = (char *)XMALLOC(sizeof(char) * total_length);

  *buf = 0;
  for (arg = argv; *arg != NULL; arg++)
  {
#ifdef HAVE_STRLCAT
    strlcat(buf, *arg, total_length);
    strlcat(buf, " ", total_length);
#else
    size_t current_len = strlen(buf);
    size_t remaining = total_length - current_len - 1;
    strncat(buf, *arg, remaining);
    current_len = strlen(buf);
    remaining = total_length - current_len - 1;
    if (remaining > 0)
      strncat(buf, " ", remaining);
#endif
  }

  return buf;
}

/****
 *
 * Debug-aware memory allocation with tracking
 *
 * DESCRIPTION:
 *   Wrapper around malloc() that provides debug tracking, error handling,
 *   and optional memory leak detection. Maintains allocation records
 *   in debug builds for comprehensive memory management analysis.
 *
 * PARAMETERS:
 *   size - Number of bytes to allocate
 *   filename - Source file making allocation (for debugging)
 *   linenumber - Line number of allocation (for debugging)
 *
 * RETURNS:
 *   Pointer to allocated memory on success
 *   NULL if allocation fails (program may exit)
 *
 * SIDE EFFECTS:
 *   - Allocates memory using system malloc()
 *   - In debug builds, adds allocation to tracking list
 *   - May terminate program on allocation failure
 *   - Updates allocation statistics
 *
 * DEBUG FEATURES:
 *   - Tracks allocation location and size
 *   - Enables memory leak detection
 *   - Provides allocation statistics
 *
 ****/

void *xmalloc_(const int size, const char *filename, const int linenumber)
{
  void *result;
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *d_result;
#endif

  /* allocate buf */
  result = malloc(size);
  if (result == NULL)
  {
#ifdef PINEAPPLE
    fprintf(stderr, "PINEAPPLE\n");
#endif
    fprintf(stderr, "out of memory (%d at %s:%d)!\n", size, filename,
            linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(EXIT_FAILURE);
  }

#ifdef MEM_DEBUG
  d_result = malloc(sizeof(struct Mem_s));
  if (d_result == NULL)
  {
    fprintf(stderr, "out of memory (%lu at %s:%d)!\n", sizeof(struct Mem_s),
            filename, linenumber);
    XFREE_ALL();
    exit(EXIT_FAILURE);
  }
  /* clean it */
  bzero(d_result, sizeof(struct Mem_s));

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "%p malloc() called from %s:%d (%d bytes)\n", result,
          filename, linenumber, size);
#endif

  /* link into the buffer chain */
  if (tail == NULL)
  {
    head = d_result;
    tail = d_result;
  }
  else
  {
    tail->next = d_result;
    d_result->prev = tail;
    d_result->next = NULL;
    tail = d_result;
  }

  /* associate the debug object with the object */
  d_result->buf_ptr = (void *)result;
  d_result->buf_size = size;
#endif

  bzero(result, size);

#ifdef MEM_DEBUG
  d_result->status = MEM_D_STAT_CLEAN;
#endif

  return result;
}

/****
 *
 * copy from one place to another
 *
 ****/

void *xmemcpy_(void *d_ptr, void *s_ptr, const int size, const char *filename,
               const int linenumber)
{
  void *result;
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *mem_ptr;
  PRIVATE int source_size;
  PRIVATE int dest_size;
#endif

  if (s_ptr == NULL)
  {
    fprintf(stderr, "memcpy called with NULL source pointer at %s:%d\n",
            filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }
  if (d_ptr == NULL)
  {
    fprintf(stderr, "memcpy called with NULL dest pointer at %s:%d\n", filename,
            linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

  /* XXX need to search the chain and see if a pointer is between the start and size */
#ifdef MEM_DEBUG
  /* search for debug mem objects */
  source_size = dest_size = 0;
  mem_ptr = head;
  while (mem_ptr != NULL)
  {
    if (mem_ptr->buf_ptr == d_ptr)
    {
      /* found the dest */
      dest_size = mem_ptr->buf_size;
    }
    else if (mem_ptr->buf_ptr == s_ptr)
    {
      /* found the source */
      source_size = mem_ptr->buf_size;
    }
    mem_ptr = mem_ptr->next;
  }

  if (dest_size > 0)
  {
    if (dest_size < size)
    {
      /* attempting to copy too much data into dest */
      fprintf(stderr,
              "memcpy called with size (%d) larger than dest buffer %p (%d) at %s:%d\n",
              size, d_ptr, dest_size, filename, linenumber);
      XFREE_ALL();
      exit(EXIT_FAILURE);
    }

    if (source_size > 0)
    {
      if (source_size < size)
      {
        /* attempting to copy too much data from source */
        fprintf(stderr,
                "memcpy called with size (%d) larger than source buffer %p (%d) at %s:%d\n",
                size, s_ptr, source_size, filename, linenumber);
        XFREE_ALL();
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      /* could not find source buffer */
#ifdef SHOW_MEM_DEBUG
      fprintf(stderr,
              "%p could not find source buffer at %s:%d called from %s%d\n",
              s_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
    }
  }
  else
  {
    /* could not find dest buffer */
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr,
            "%p could not find dest buffer at %s:%d called from %s:%d\n",
            d_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
  }
#endif

  if (s_ptr < d_ptr)
  {
    if (s_ptr + size >= d_ptr)
    {
      /* overlap, use memmove */
      result = memmove(d_ptr, s_ptr, size);
    }
    else
    {
      /* no overlap, use memcpy */
      result = memcpy(d_ptr, s_ptr, size);
    }
  }
  else if (s_ptr > d_ptr)
  {
    if (d_ptr + size >= s_ptr)
    {
      /* overlap, use memmove */
      result = memmove(d_ptr, s_ptr, size);
    }
    else
    {
      /* no overlap, use memcpy */
      result = memcpy(d_ptr, s_ptr, size);
    }
  }
  else
  {
    /* source and dest are the same, freak out */
    fprintf(stderr, "memcpy() called with source == dest at %s:%d\n", filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "%p memcpy() called from %s:%d (%d bytes)\n", result, filename, linenumber, size);
#endif

  return result;
}

/****
 *
 * copy from one place to another
 *
 ****/

char *xmemncpy_(char *d_ptr, const char *s_ptr, const size_t len __attribute__((unused)),
                const int size, const char *filename __attribute__((unused)), const int linenumber __attribute__((unused)))
{
  char *result;
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *mem_ptr;
  PRIVATE int source_size;
  PRIVATE int dest_size;
#endif

  if (s_ptr == NULL)
  {
    fprintf(stderr, "memcpy called with NULL source pointer at %s:%d\n",
            filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }
  if (d_ptr == NULL)
  {
    fprintf(stderr, "memcpy called with NULL dest pointer at %s:%d\n", filename,
            linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

#ifdef MEM_DEBUG
  /* search for debug mem objects */
  source_size = dest_size = 0;
  mem_ptr = head;
  while (mem_ptr != NULL)
  {
    if (mem_ptr->buf_ptr == d_ptr)
    {
      /* found the dest */
      dest_size = mem_ptr->buf_size;
    }
    else if (mem_ptr->buf_ptr == s_ptr)
    {
      /* found the source */
      source_size = mem_ptr->buf_size;
    }
    mem_ptr = mem_ptr->next;
  }

  if (dest_size > 0)
  {
    if (dest_size < size)
    {
      /* attempting to copy too much data into dest */
      fprintf(stderr,
              "memcpy called with size (%d) larger than dest buffer %p (%d) at %s:%d\n",
              size, d_ptr, dest_size, filename, linenumber);
      XFREE_ALL();
      exit(1);
    }

    if (source_size > 0)
    {
      if (source_size < size)
      {
        /* attempting to copy too much data from source */
        fprintf(stderr,
                "memcpy called with size (%d) larger than source buffer %p (%d) at %s:%d\n",
                size, s_ptr, source_size, filename, linenumber);
        XFREE_ALL();
        exit(1);
      }
    }
    else
    {
      /* could not find source buffer */
#ifdef SHOW_MEM_DEBUG
      fprintf(stderr,
              "%p could not find source buffer at %s:%d called from %s%d\n",
              s_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
    }
  }
  else
  {
    /* could not find dest buffer */
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr,
            "%p could not find dest buffer at %s:%d called from %s:%d\n",
            d_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
  }
#endif

  if (s_ptr < d_ptr)
  {
    if (s_ptr + size >= d_ptr)
    {
      /* overlap, use memmove */
      result = memmove(d_ptr, s_ptr, size);
    }
    else
    {
      /* no overlap, use memcpy */
      result = memcpy(d_ptr, s_ptr, size);
    }
  }
  else if (s_ptr > d_ptr)
  {
    if (d_ptr + size >= s_ptr)
    {
      /* overlap, use memmove */
      result = memmove(d_ptr, s_ptr, size);
    }
    else
    {
      /* no overlap, use memcpy */
      result = memcpy(d_ptr, s_ptr, size);
    }
  }
  else
  {
    /* source and dest are the same, freak out */
    fprintf(stderr, "memcpy() called with source == dest at %s:%d\n", filename,
            linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "%p memcpy() called from %s:%d (%d bytes)\n", result, filename, linenumber, size);
#endif

  return result;
}

/****
 *
 * set memory area
 *
 ****/

void *xmemset_(void *ptr, const char value, const int size,
               const char *filename, const int linenumber)
{
  void *result;

  if (ptr == NULL)
  {
    fprintf(stderr, "memset() called with NULL ptr at %s:%d\n", filename,
            linenumber);
    quit = TRUE;
    exit(1);
  }

  if (value == 0)
  {
    bzero(ptr, size);
    result = ptr;
  }
  else
  {
    result = memset(ptr, value, size);
  }

#ifdef MEM_DEBUG
  fprintf(stderr, "%p memset %s:%d (%d bytes)\n", result, filename, linenumber, size);
#endif

  return result;
}

/****
 *
 * compare memory
 *
 ****/

int xmemcmp_(const void *s1, const void *s2, size_t n, const char *filename,
             const int linenumber)
{
  int result;

  if (s1 == NULL || s2 == NULL)
  {
    fprintf(stderr, "memcmp() called with NULL ptr at %s:%d\n", filename,
            linenumber);
    quit = TRUE;
    exit(1);
  }

  result = memcmp(s1, s2, n);

#ifdef MEM_DEBUG
  fprintf(stderr, "%p memcmp against %p %s:%d (%ld bytes)\n", s1, s2, filename, linenumber, n);
#endif

  return result;
}

/****
 *
 * Allocate memory. Checks the return value, aborts if no more memory is
 *available
 *
 ****/

void *xrealloc_(void *ptr, int size, const char *filename, const int linenumber)
{
  void *result;
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *d_result;
  PRIVATE struct Mem_s *d_ptr;
  PRIVATE int found = FALSE;
  PRIVATE int d_size = 0;
#endif

  if (ptr == NULL)
    result = malloc(size);
  else
    result = realloc(ptr, size);

#ifdef MEM_DEBUG
  fprintf(stderr, "%p realloc %s:%d (%d bytes)\n", result, filename, linenumber, size);
#endif

  if (result == NULL)
  {
    fprintf(stderr, "out of memory (%d at %s:%d)!\n", size, filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(EXIT_FAILURE);
  }

#ifdef MEM_DEBUG
  d_ptr = head;
  while (d_ptr != NULL)
  {
    if (d_ptr->buf_ptr == ptr)
    {
      /* found debug object */
      found = TRUE;
      if (d_ptr->prev != NULL)
      {
        d_ptr->prev->next = d_ptr->next;
      }
      else
      {
        head = (void *)d_ptr->next;
      }
      if (d_ptr->next != NULL)
      {
        d_ptr->next->prev = d_ptr->prev;
      }
      else
      {
        tail = d_ptr->prev;
      }
      d_size = d_ptr->buf_size;
      free(d_ptr);
      d_ptr = NULL;
    }
    else
    {
      d_ptr = d_ptr->next;
    }
  }

  if (!found)
  {
    fprintf(stderr, "realloc() called with %p ptr but not found in debug object list at %s:%d\n", ptr, filename, linenumber);
  }

  d_result = malloc(sizeof(struct Mem_s));
  if (d_result == NULL)
  {
    fprintf(stderr, "out of memory (%lu at %s:%d)!\n", sizeof(struct Mem_s), filename, linenumber);
    XFREE_ALL();
    exit(EXIT_FAILURE);
  }
  /* clean it */
  bzero(d_result, sizeof(struct Mem_s));

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "%p realloc() called from %s:%d (%d bytes)\n", result, filename, linenumber, size);
#endif

  /* link into the buffer chain */
  if (tail == NULL)
  {
    head = d_result;
    tail = d_result;
  }
  else
  {
    tail->next = d_result;
    d_result->prev = tail;
    d_result->next = NULL;
    tail = d_result;
  }

  /* associate the debug object with the object */
  d_result->buf_ptr = (void *)result;
  d_result->buf_size = size;
#endif

  return result;
}

/****
 *
 * Free memory. Merely a wrapper for the case that we want to keep track of
 *allocations.
 *
 ****/

void xfree_(void *ptr, const char *filename, const int linenumber)
{
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *d_ptr;
  PRIVATE int found = FALSE;
  PRIVATE int size = 0;
#endif

  if (ptr == NULL)
  {
    fprintf(stderr, "free() called with NULL ptr at %s:%d\n", filename,
            linenumber);
    exit(1);
  }

#ifdef MEM_DEBUG
  d_ptr = head;
  while (d_ptr != NULL)
  {
    if (d_ptr->buf_ptr == ptr)
    {
      /* found debug object */
      found = TRUE;
      if (d_ptr->prev != NULL)
      {
        d_ptr->prev->next = d_ptr->next;
      }
      else
      {
        head = (void *)d_ptr->next;
      }
      if (d_ptr->next != NULL)
      {
        d_ptr->next->prev = d_ptr->prev;
      }
      else
      {
        tail = d_ptr->prev;
      }
      size = d_ptr->buf_size;
      free(d_ptr);
      d_ptr = NULL;
    }
    else
    {
      d_ptr = d_ptr->next;
    }
  }

  if (!found)
  {
    fprintf(stderr, "free() called with %p ptr but not found in debug object list at %s:%d\n", ptr, filename, linenumber);
  }
#endif

#ifdef SHOW_MEM_DEBUG
#ifdef MEM_DEBUG
  fprintf(stderr, "%p free() called from %s:%d (%d bytes)\n", ptr, filename, linenumber, size);
#else
  fprintf(stderr, "%p free() called from %s:%d\n", ptr, filename, linenumber);
#endif
#endif

  free(ptr);
}

/****
 *
 * free all known buffers
 *
 ****/

#ifdef MEM_DEBUG
void xfree_all_(const char *filename, const int linenumber)
{
  PRIVATE struct Mem_s *d_ptr;
  PRIVATE int size = 0;
  PRIVATE struct timespec pause_time;

  /* mutex trylock pause */
  pause_time.tv_sec = 0;
  pause_time.tv_nsec = 500;

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "xfree_all() called from %s:%d\n", filename, linenumber);
#endif

  /* pop all buffers off the list */
  while ((d_ptr = head) != NULL)
  {
    head = d_ptr->next;
    if (d_ptr->buf_ptr != NULL)
    {
#ifdef SHOW_MEM_DEBUG
      fprintf(stderr, "%p free %s:%d (%d bytes)\n", d_ptr->buf_ptr, filename, linenumber, d_ptr->buf_size);
#endif
      free(d_ptr->buf_ptr);
      d_ptr->buf_ptr = NULL;
    }
    free(d_ptr);
  }

  return;
}
#endif

/****
 *
 * Dup a string
 *
 ****/

char *xstrdup_(const char *str, const char *filename __attribute__((unused)), const int linenumber __attribute__((unused)))
{
  char *res;

  res = strdup(str);

#ifdef MEM_DEBUG
  fprintf(stderr, "%p malloc %s:%d (%ld) bytes, strdup\n", res, filename, linenumber, strlen(str) + 1);
#endif

  return res;
}

/****
 *
 * grow or shrink an array
 *
 ****/

void xgrow_(void **old, int elementSize, int *oldCount, int newCount, char *filename, const int linenumber)
{
  void *tmp;
  int size;

  size = newCount * elementSize;
  if (size == 0)
    tmp = NULL;
  else
  {
    tmp = malloc(size);

#ifdef MEM_DEBUG
    fprintf(stderr, "%p malloc %s:%d (grow)\n", tmp, filename, linenumber);
#endif

    if (tmp == NULL)
    {
      fprintf(stderr, "out of memory (%d at %s:%d)!\n", size, filename, linenumber);
      quit = TRUE;
      exit(1);
    }
    memset(tmp, 0, size);
    if (*oldCount > newCount)
      *oldCount = newCount;
    memcpy(tmp, *old, elementSize * (*oldCount));
  }

  if (*old != NULL)
  {
#ifdef MEM_DEBUG
    fprintf(stderr, "%p free %s:%d (grow)\n", *old, filename, linenumber);
#endif
    free(*old);
  }
  *old = tmp;
  *oldCount = newCount;
}

/****
 *
 * wraper around strcpy
 *
 ****/

char *xstrcpy_(char *d_ptr, const char *s_ptr, const char *filename, const int linenumber)
{
  void *result;
  PRIVATE int size;
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *mem_ptr;
  PRIVATE int source_size;
  PRIVATE int dest_size;
#endif

  if (s_ptr == NULL)
  {
    fprintf(stderr, "strcpy called with NULL source pointer at %s:%d\n",
            filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }
  if (d_ptr == NULL)
  {
    fprintf(stderr, "strcpy called with NULL dest pointer at %s:%d\n", filename,
            linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

  if ((size = (strlen(s_ptr) + 1)) == 0)
  {
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr, "strcpy called with zero length source pointer at %s:%d\n",
            filename, linenumber);
#endif
    d_ptr[0] = 0;
    return d_ptr;
  }

#ifdef MEM_DEBUG
  /* search for debug mem objects */
  source_size = dest_size = 0;
  mem_ptr = head;
  while (mem_ptr != NULL)
  {
    if (mem_ptr->buf_ptr == d_ptr)
    {
      /* found the dest */
      dest_size = mem_ptr->buf_size;
    }
    else if (mem_ptr->buf_ptr == s_ptr)
    {
      /* found the source */
      source_size = mem_ptr->buf_size;
    }
    mem_ptr = mem_ptr->next;
  }

  if (dest_size > 0)
  {
    if (dest_size < size)
    {
      /* attempting to copy too much data into dest */
      fprintf(stderr,
              "strcpy called with size (%d) larger than dest buffer %p (%d) at %s:%d\n",
              size, d_ptr, dest_size, filename, linenumber);
      XFREE_ALL();
      exit(1);
    }

    if (source_size > 0)
    {
      if (source_size < size)
      {
        /* attempting to copy too much data from source */
        fprintf(stderr,
                "strcpy called with size (%d) larger than source buffer %p (%d) at %s:%d\n",
                size, s_ptr, source_size, filename, linenumber);
        XFREE_ALL();
        exit(1);
      }
    }
    else
    {
      /* could not find source buffer */
#ifdef SHOW_MEM_DEBUG
      fprintf(stderr,
              "%p could not find source buffer at %s:%d called from %s%d\n",
              s_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
    }
  }
  else
  {
    /* could not find dest buffer */
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr,
            "%p could not find dest buffer at %s:%d called from %s:%d\n",
            d_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
  }
#endif

  if (s_ptr < d_ptr)
  {
    if (s_ptr + size >= d_ptr)
    {
      /* overlap, use memmove */
      result = memmove(d_ptr, s_ptr, size);
    }
    else
    {
      /* no overlap, use memcpy */
      result = memcpy(d_ptr, s_ptr, size);
    }
  }
  else if (s_ptr > d_ptr)
  {
    if (d_ptr + size >= s_ptr)
    {
      /* overlap, use memmove */
      result = memmove(d_ptr, s_ptr, size);
    }
    else
    {
      /* no overlap, use memcpy */
      result = memcpy(d_ptr, s_ptr, size);
    }
  }
  else
  {
    /* source and dest are the same, freak out */
    fprintf(stderr, "strcpy() called with source == dest at %s:%d\n", filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }
  d_ptr[size - 1] = 0;

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "%p strcpy() called from %s:%d (%d bytes)\n", result, filename, linenumber, size);
#endif

  return result;
}

/****
 *
 * wraper around strncpy
 *
 ****/

char *xstrncpy_(char *d_ptr, const char *s_ptr, const size_t len, const char *filename, const int linenumber)
{
  char *result;
  PRIVATE size_t size;
#ifdef MEM_DEBUG
  PRIVATE struct Mem_s *mem_ptr;
  PRIVATE int source_size;
  PRIVATE int dest_size;
#endif

  /* check for null source pointer */
  if (s_ptr == NULL)
  {
    fprintf(stderr, "strncpy called with NULL source pointer at %s:%d\n", filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

  /* check for null dest pointer */
  if (d_ptr == NULL)
  {
    fprintf(stderr, "strncpy called with NULL dest pointer at %s:%d\n", filename, linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

  /* check size of len arg */
  if (len == 0)
  {
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr, "strncpy called with zero copy length at %s:%d\n", filename, linenumber);
#endif
    d_ptr[0] = 0;
    return d_ptr;
  }

  /* check size of source string */
  if ((size = (strnlen(s_ptr,len-1) + 1)) == 0)
  {
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr, "strncpy called with zero length source pointer at %s:%d\n", filename, linenumber);
#endif
    d_ptr[0] = 0;
    return d_ptr;
  }

  /* check if source string lenght >= length arg */
  if (size >= len)
  {
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr, "strncpy called with source string >= length arg at %s:%d\n", filename, linenumber);
#endif
  }

#ifdef MEM_DEBUG
  /* search for debug mem objects */
  source_size = dest_size = 0;
  mem_ptr = head;
  while (mem_ptr != NULL)
  {
    if (mem_ptr->buf_ptr == d_ptr)
    {
      /* found the dest */
      dest_size = mem_ptr->buf_size;
    }
    else if (mem_ptr->buf_ptr == s_ptr)
    {
      /* found the source */
      source_size = mem_ptr->buf_size;
    }
    mem_ptr = mem_ptr->next;
  }

  if (dest_size > 0)
  {
    if (dest_size < len)
    {
      /* attempting to copy too much data into dest */
      fprintf(stderr,
              "strncpy called with size (%lu) larger than dest buffer %p (%d) at %s:%d\n",
              len, d_ptr, dest_size, filename, linenumber);
      XFREE_ALL();
      exit(1);
    }

    if (source_size > 0)
    {
      if (source_size < len)
      {
        /* attempting to copy too much data from source */
        fprintf(stderr,
                "strncpy called with size (%lu) larger than source buffer %p (%d) at %s:%d\n",
                len, s_ptr, source_size, filename, linenumber);
        XFREE_ALL();
        exit(1);
      }
    }
    else
    {
      /* could not find source buffer */
#ifdef SHOW_MEM_DEBUG
      fprintf(stderr,
              "%p could not find source buffer at %s:%d called from %s:%d\n",
              s_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
    }
  }
  else
  {
    /* could not find dest buffer */
#ifdef SHOW_MEM_DEBUG
    fprintf(stderr,
            "%p could not find dest buffer at %s:%d called from %s:%d\n",
            d_ptr, __FILE__, __LINE__, filename, linenumber);
#endif
  }
#endif

  if (s_ptr != d_ptr)
    strncpy( d_ptr, s_ptr, len );
  else
  {
    /* source and dest are the same, freak out */
    fprintf(stderr, "strncpy() called with source == dest at %s:%d\n", filename,
            linenumber);
#ifdef MEM_DEBUG
    XFREE_ALL();
#endif
    exit(1);
  }

#ifdef SHOW_MEM_DEBUG
  fprintf(stderr, "%p strncpy() called from %s:%d (%d bytes)\n",
          result, filename, linenumber, size);
#endif

  return result;
}
