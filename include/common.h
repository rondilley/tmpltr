/*****
 * 
 * Common definitions, structures and unions
 * 
 * Copyright (c) 2011-2023, Ron Dilley
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
  int mode;
  int facility;
  int priority;
  int alarm_count;
  time_t current_time;
  pid_t cur_pid;
  FILE *outFile_st;
} Config_t;

#endif	/* end of COMMON_H */

