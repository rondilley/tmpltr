/****
 *
 * Copyright (c) 2011-2014, Ron Dilley
 * All rights reserved.
 *
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of Uberadmin/BaraCUDA/Nightingale nor the names of
*     its contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****/

/****
 *
 * defines
 *
 ****/

/* turn on priority names */
#define SYSLOG_NAMES

/****
 *
 * includes
 *
 ****/

#include "util.h"

/****
 *
 * local variables
 *
 ****/

PRIVATE char *restricted_environ[] = {
  "IFS= \t\n",
  "PATH= /bin:/usr/bin",
  0
};
PRIVATE char *preserve_environ[] = {
  "TZ",
  0
};

#ifdef SOLARIS

#define INTERNAL_NOPRI  0x10    /* the "no priority" priority */
                                /* mark "facility" */
#define INTERNAL_MARK   LOG_MAKEPRI(LOG_NFACILITIES, 0)
typedef struct _code {
        char    *c_name;
        int     c_val;
} CODE;

CODE prioritynames[] =
  {
    { "alert", LOG_ALERT },
    { "crit", LOG_CRIT },
    { "debug", LOG_DEBUG },
    { "emerg", LOG_EMERG },
    { "err", LOG_ERR },
    { "error", LOG_ERR },               /* DEPRECATED */
    { "info", LOG_INFO },
    { "none", INTERNAL_NOPRI },         /* INTERNAL */
    { "notice", LOG_NOTICE },
    { "panic", LOG_EMERG },             /* DEPRECATED */
    { "warn", LOG_WARNING },            /* DEPRECATED */
    { "warning", LOG_WARNING },
    { NULL, -1 }
  };
#endif

/****
 *
 * external global variables
 *
 ****/

extern Config_t *config;
#ifndef SOLARIS
extern CODE prioritynames[];
#endif
extern char **environ;

/****
 *
 * functions
 *
 ****/

/****
 *
 * display output
 *
 ****/

int display( int level, char *format, ... ) {
  PRIVATE va_list args;
  PRIVATE char tmp_buf[SYSLOG_MAX];
  PRIVATE int i;

  va_start( args, format );
  vsprintf( tmp_buf, format, args );
  if ( tmp_buf[strlen(tmp_buf)-1] == '\n' ) {
    tmp_buf[strlen(tmp_buf)-1] = 0;
  }
  va_end( args );

  if ( config->mode != MODE_INTERACTIVE ) {
    /* display info via syslog */
    syslog( level, tmp_buf );
  } else {
    if ( level <= LOG_ERR ) {
      /* display info via stderr */
      for ( i = 0; prioritynames[i].c_name != NULL; i++ ) {
	if ( prioritynames[i].c_val == level ) {
	  fprintf( stderr, "%s[%u] - %s\n", prioritynames[i].c_name, config->cur_pid, tmp_buf );
	  return TRUE;
	}
      }
    } else {
      /* display info via stdout */
      for ( i = 0; prioritynames[i].c_name != NULL; i++ ) {
	if ( prioritynames[i].c_val == level ) {
	  printf( "%s[%u] - %s\n", prioritynames[i].c_name, config->cur_pid, tmp_buf );
	  return TRUE;
	}
      }
    }
  }

  return FAILED;
}

/****
 *
 * open file descriptor for the null device
 *
 ****/

PUBLIC int open_devnull( int fd ) {
  FILE *f_st = 0;

  if ( fd EQ 0 ) f_st = freopen( DEV_NULL, "rb", stdin );
  else if ( fd EQ 1 ) f_st = freopen( DEV_NULL, "wb", stdout );
  else if ( fd EQ 2 ) f_st = freopen( DEV_NULL, "wb", stderr );
  return ( f_st && fileno( f_st ) EQ fd );
}

/****
 *
 * check to see if dir is safe
 *
 ****/

int is_dir_safe( const char *dir ) {
  DIR *fd, *start;
  int rc = FAILED;
  char new_dir[PATH_MAX+1];
  uid_t uid;
  struct stat f, l;

  if ( !( start = opendir( "." ) ) ) return FAILED;
  if ( lstat( dir, &l ) == FAILED ) {
    closedir( start );
    return FAILED;
  }
  uid = geteuid();

  do {
    if ( chdir( dir ) EQ FAILED ) break;
    if ( !( fd = opendir( "." ) ) ) break;

#ifdef LINUX
    if ( fstat( dirfd( fd ), &f ) EQ FAILED ) {
#elif MACOS
    if ( fstat( fd->__dd_fd, &f ) EQ FAILED ) {
#elif CYGWIN
    if ( fstat( fd->__d_fd, &f ) EQ FAILED ) {
#elif OPENBSD
    if ( fstat( dirfd( fd ), &f ) EQ FAILED ) {

#else
    if ( fstat( fd->dd_fd, &f ) EQ FAILED ) {
#endif

      closedir( fd );
      break;
    }
    closedir( fd );

    if ( l.st_mode != f.st_mode || l.st_ino != f.st_ino || l.st_dev != f.st_dev )
      break;
    if ( ( f.st_mode & ( S_IWOTH | S_IWGRP ) ) || ( f.st_uid && f.st_uid != uid ) ) {
      rc = 0;
      break;
    }
    dir = "..";
    if ( lstat( dir, &l ) EQ FAILED ) break;
    if ( !getcwd( new_dir, PATH_MAX + 1 ) ) break;
  } while ( new_dir[1] ); /* new_dir[0] will always be a slash */
  if ( !new_dir[1] ) rc = 1;

#ifdef LINUX
  fchdir( dirfd( start ) );
#elif MACOS
  fchdir( start->__dd_fd );
#elif CYGWIN
  fchdir( start->__d_fd );
#elif OPENBSD
  fchdir( dirfd( start ) );
#else
  fchdir( start->dd_fd );
#endif

  closedir( start );
  return rc;
}

/****
 *
 * create pid file
 *
 ****/

int create_pid_file( const char *filename ) {
  int fd;
  FILE *lockfile;
  size_t len;
  pid_t pid;

  /* remove old pid file if it exists */
  cleanup_pid_file( filename );
  if ( ( fd = safe_open( filename ) ) < 0 ) {
    display( LOG_ERR, "Unable to open pid file [%s]", filename );
    return FAILED;
  }
  if ( ( lockfile = fdopen(fd, "w") ) EQ NULL ) {
    display( LOG_ERR, "Unable to fdopen() pid file [%d]", fd );
    return FAILED;
  }
  pid = getpid();
  if (fprintf( lockfile, "%ld\n", (long)pid) < 0) {
    display( LOG_ERR, "Unable to write pid to file [%s]", filename );
    fclose( lockfile );
    return FAILED;
  }
  if ( fflush( lockfile ) EQ EOF ) {
    display( LOG_ERR, "fflush() failed [%s]", filename );
    fclose( lockfile );
    return FAILED;
  }

  fclose( lockfile );
  return TRUE;
}

/****
 *
 * safely open a file for writing
 *
 ****/

static int safe_open( const char *filename ) {
  int fd;
  struct stat sb;
  XMEMSET( &sb, 0, sizeof( struct stat ) );
                                                                 
  if ( lstat(filename, &sb) EQ FAILED ) {
    if (errno != ENOENT)
      return( FAILED );
  } else if ( ( sb.st_mode & S_IFREG) EQ 0 ) {
    errno = EOPNOTSUPP;
    return ( FAILED );
  }

  unlink( filename );
  fd = open( filename, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );

  return (fd);
}

/****
 *
 * cleaup pid file
 *
 ****/

static void cleanup_pid_file( const char *filename ) {
  if ( strlen( filename ) > 0 ) {
    unlink( filename );
  }
}

/****
 *
 * sanitize environment
 *
 ****/

void sanitize_environment( void ) {
  int i;
  char **new_environ;
  char *ptr, *value, *var;
  size_t arr_size = 1;
  size_t arr_ptr = 0;
  size_t len;
  size_t new_size = 0;

  for( i = 0; (var = restricted_environ[i]) != 0; i++ ) {
    new_size += strlen( var ) + 1;
    arr_size++;
  }

  for ( i = 0; (var = preserve_environ[i]) != 0; i++ ) {
    if ( !(value = getenv(var))) continue;
    new_size += strlen( var ) + strlen( value ) + 2;
    arr_size++;
  }

  new_size += ( arr_size * sizeof( char * ) );
  new_environ = (char **)XMALLOC( new_size );
  new_environ[arr_size - 1] = 0;
  ptr = ( char * )new_environ + (arr_size * sizeof(char *));
  for ( i = 0; ( var = restricted_environ[i] ) != 0; i++ ) {
    new_environ[arr_ptr++] = ptr;
    len = strlen( var );
    XMEMCPY( ptr, var, len + 1 );
    ptr += len + 1;
  }

  for ( i = 0; ( var = preserve_environ[i] ) != 0; i++ ) {
    if ( !( value = getenv( var ) ) ) continue;
    new_environ[arr_ptr++] = ptr;
    len = strlen( var );
    XMEMCPY( ptr, var, len );
    *(ptr + len + 1 ) = '=';
    XMEMCPY( ptr + len + 2, value, strlen( value ) + 1 );
    ptr += len + strlen( value ) + 2;
  }

  environ = new_environ;
}
