/*****
 * 
 * Common definitions, structures and unions
 * 
 * Copyright (c) 2011-2025, Ron Dilley
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

#ifndef COMMON_H
#define COMMON_H 1

/****
 *
 * defines
 *
 ****/

#define FAILED -1
#define FALSE 0
#define TRUE 1

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1
#endif

#define MODE_DAEMON 0
#define MODE_INTERACTIVE 1
#define MODE_DEBUG 2

#define PRIVATE static
#define PUBLIC
#define EQ ==
#define NE !=

#ifndef PATH_MAX
# ifdef MAXPATHLEN
#  define PATH_MAX MAXPATHLEN
# else
#  define PATH_MAX 1024
# endif
#endif

#ifdef __cplusplus
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else /* !__cplusplus */
# define BEGIN_C_DECLS
# define END_C_DECLS
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
# error something is messed up
#endif

/****
 *
 * Character Classification Constants
 *
 ****/

/* Character classification bits for fast lookup table */
#define CHAR_ALPHA  0x01  /* Alphabetic character (a-z, A-Z) */
#define CHAR_DIGIT  0x02  /* Digit character (0-9) */
#define CHAR_ALNUM  0x03  /* Alpha or numeric (combination of above) */
#define CHAR_XDIGIT 0x04  /* Hexadecimal digit (0-9, a-f, A-F) */
#define CHAR_PUNCT  0x08  /* Punctuation character */
#define CHAR_SPACE  0x10  /* Whitespace character */
#define CHAR_CNTRL  0x20  /* Control character */
#define CHAR_PRINT  0x40  /* Printable character */

/* Fast character classification macros using lookup table */
extern const unsigned char char_class_table[256];

#define FAST_ISALPHA(c)  (char_class_table[(unsigned char)(c)] & CHAR_ALPHA)
#define FAST_ISDIGIT(c)  (char_class_table[(unsigned char)(c)] & CHAR_DIGIT)
#define FAST_ISALNUM(c)  (char_class_table[(unsigned char)(c)] & CHAR_ALNUM)
#define FAST_ISXDIGIT(c) (char_class_table[(unsigned char)(c)] & CHAR_XDIGIT)
#define FAST_ISPUNCT(c)  (char_class_table[(unsigned char)(c)] & CHAR_PUNCT)
#define FAST_ISSPACE(c)  (char_class_table[(unsigned char)(c)] & CHAR_SPACE)
#define FAST_ISCNTRL(c)  (char_class_table[(unsigned char)(c)] & CHAR_CNTRL)
#define FAST_ISPRINT(c)  (char_class_table[(unsigned char)(c)] & CHAR_PRINT)

/****
 *
 * enums & typedefs
 *
 ****/

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

/* prog config */

typedef struct {
  uid_t starting_uid;
  uid_t uid;
  gid_t gid;
  char *home_dir;
  char *log_dir;
  FILE *syslog_st;
  char *hostname;
  char *domainname;
  int debug;
  int greedy;
  int cluster;
  int clusterDepth;
  int chain;
  int match;
  int no_output;  /* Skip printing templates at end */
  int mode;
  int facility;
  int priority;
  int alarm_count;
  time_t current_time;
  pid_t cur_pid;
  FILE *outFile_st;
  int parser_type;  /* Parser type selection */
} Config_t;

#endif	/* end of COMMON_H */

