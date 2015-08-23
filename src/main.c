/*****
 *
 * Description: Main Functions
 * 
 * Copyright (c) 2008-2015, Ron Dilley
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
 *.
 ****/

#include "main.h"

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

PUBLIC int quit = FALSE;
PUBLIC int reload = FALSE;
PUBLIC Config_t *config = NULL;

/****
 *
 * external variables
 *
 ****/

extern int errno;
extern char **environ;

/****
 *
 * main function
 *
 ****/

int main(int argc, char *argv[]) {
  FILE *inFile = NULL, *outFile = NULL;
  char inBuf[8192];
  char outFileName[PATH_MAX];
  PRIVATE int c = 0, i, ret;

#ifndef DEBUG
  struct rlimit rlim;

  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit( RLIMIT_CORE, &rlim );
#endif

  /* setup config */
  config = ( Config_t * )XMALLOC( sizeof( Config_t ) );
  XMEMSET( config, 0, sizeof( Config_t ) );

  /* force mode to forground */
  config->mode = MODE_INTERACTIVE;

  /* store current pid */
  config->cur_pid = getpid();

  /* get real uid and gid in prep for priv drop */
  config->gid = getgid();
  config->uid = getuid();

  while (1) {
    int this_option_optind = optind ? optind : 1;
#ifdef HAVE_GETOPT_LONG
    int option_index = 0;
    static struct option long_options[] = {
      {"chain", no_argument, 0, 'C' },
      {"cluster", no_argument, 0, 'c' },
      {"greedy", no_argument, 0, 'g' },
      {"version", no_argument, 0, 'v' },
      {"debug", required_argument, 0, 'd' },
      {"help", no_argument, 0, 'h' },
      {"cnum", required_argument, 0, 'n' },
      {"templates", required_argument, 0, 't' },
      {"write", required_argument, 0, 'w' },
      {"match", required_argument, 0, 'm' },
      {"matchfile", required_argument, 0, 'M' },
      {"line", required_argument, 0, 'l' },
      {"linefile", required_argument, 0, 'L' },
      {0, no_argument, 0, 0}
    };
    c = getopt_long(argc, argv, "vd:hn:t:w:cCgm:M:l:L:", long_options, &option_index);
#else
    c = getopt( argc, argv, "vd:htn::w:cCgm:M:l:L:" );
#endif

    if (c EQ -1)
      break;

    switch (c) {

    case 'v':
      /* show the version */
      print_version();
      return( EXIT_SUCCESS );

    case 'c':
      /* enable argument clustering */
      config->cluster = TRUE;
      break;

    case 'C':
      /* enable log chaining */
      config->cluster = TRUE;
      config->chain = TRUE;
      break;
      
    case 'd':
      /* show debig info */
      config->debug = atoi( optarg );
      break;

    case 'g':
      /* ignore quotes */
      config->greedy = TRUE;
      break;

    case 'n':
      /* override default cluster count */
      config->clusterDepth = atoi( optarg );
      break;

    case 't':
      /* load template file */
      if ( loadTemplateFile( optarg ) != TRUE ) {
        fprintf( stderr, "ERR - Problem while loading template file\n" );
        return( EXIT_FAILURE );
      }
      break;

    case 'h':
      /* show help info */
      print_help();
      return( EXIT_SUCCESS );

    case 'w':
      /* save templates to file */
      if ( ( config->outFile_st = fopen( optarg, "w" ) ) EQ NULL ) {
	fprintf( stderr, "ERR - Unable to open template file for write [%s]\n", optarg );
	return( EXIT_FAILURE );
      }
      break;

    case 'M':
      /* load match templates from file */
      config->match = loadMatchTemplates( optarg );
      break;

    case 'm':
      /* add template to match list */
      config->match = addMatchTemplate( optarg );
      break;

    case 'L':
      /* load match lines from file and convert to templates */
      config->match = loadMatchLines( optarg );
      break;

    case 'l':
      /* convert match line and add as template */
      config->match = addMatchLine( optarg );
      break;
	
    default:
      fprintf( stderr, "Unknown option code [0%o]\n", c);
    }
  }

  /* override cluster depth */
  if ( ( config->clusterDepth <= 0 ) | ( config->clusterDepth > 10000 ) )
    config->clusterDepth = MAX_ARGS_IN_FIELD;
  
  /* check dirs and files for danger */

  if ( time( &config->current_time ) EQ -1 ) {
    display( LOG_ERR, "Unable to get current time" );

    /* cleanup buffers */
    cleanup();
    return( EXIT_FAILURE );
  }

  /* initialize program wide config options */
  config->hostname = (char *)XMALLOC( MAXHOSTNAMELEN+1 );

  /* get processor hostname */
  if ( gethostname( config->hostname, MAXHOSTNAMELEN ) != 0 ) {
    display( LOG_ERR, "Unable to get hostname" );
    strcpy( config->hostname, "unknown" );
  }

  config->cur_pid = getpid();

  /* setup current time updater */
  signal( SIGALRM, ctime_prog );
  alarm( ALARM_TIMER );

  /*
   * get to work
   */

  /* process all the files */
  while (optind < argc) {
    processFile( argv[optind++] );
  }

  if ( config->match ) {
    /* XXX should print match metrict */
  } else {
    /* print the templates we have found */
    showTemplates();
  }

  /*
   * finished with the work
   */

  cleanup();

  return( EXIT_SUCCESS );
}

/****
 *
 * display prog info
 *
 ****/

void show_info( void ) {
  fprintf( stderr, "%s v%s [%s - %s]\n", PROGNAME, VERSION, __DATE__, __TIME__ );
  fprintf( stderr, "By: Ron Dilley\n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "%s comes with ABSOLUTELY NO WARRANTY.\n", PROGNAME );
  fprintf( stderr, "This is free software, and you are welcome\n" );
  fprintf( stderr, "to redistribute it under certain conditions;\n" );
  fprintf( stderr, "See the GNU General Public License for details.\n" );
  fprintf( stderr, "\n" );
}

/*****
 *
 * display version info
 *
 *****/

PRIVATE void print_version( void ) {
  printf( "%s v%s [%s - %s]\n", PROGNAME, VERSION, __DATE__, __TIME__ );
}

/*****
 *
 * print help info
 *
 *****/

PRIVATE void print_help( void ) {
  print_version();

  fprintf( stderr, "\n" );
  fprintf( stderr, "syntax: %s [options] filename [filename ...]\n", PACKAGE );

#ifdef HAVE_GETOPT_LONG
  fprintf( stderr, " -C|--chain             show related log lines\n" );  
  fprintf( stderr, " -c|--cluster           show invariable fields in output\n" );
  fprintf( stderr, " -d|--debug (0-9)       enable debugging info\n" );
  fprintf( stderr, " -g|--greedy            ignore quotes\n" );
  fprintf( stderr, " -h|--help              this info\n" );
  fprintf( stderr, " -l|--line {line}       show all lines that match template of {line}\n");
  fprintf( stderr, " -L|--linefile {fname}  show all the lines that match templates of lines in {fname}\n");
  fprintf( stderr, " -m|--match {template}  show all lines that match {template}\n");
  fprintf( stderr, " -M|--matchfile {fname} show all the lines that match templates in {fname}\n");
  fprintf( stderr, " -n|--cnum {num}        max cluster args [default: %d]\n", MAX_ARGS_IN_FIELD );
  fprintf( stderr, " -t|--templates {file}  load templates to ignore\n" );
  fprintf( stderr, " -v|--version           display version information\n" );
  fprintf( stderr, " -w|--write {file}      save templates to file\n" );
  fprintf( stderr, " filename               one or more files to process, use '-' to read from stdin\n" );
#else
  fprintf( stderr, " -C            show related log lines\n" );
  fprintf( stderr, " -c            show invariable fields in output\n" );
  fprintf( stderr, " -d {lvl}      enable debugging info\n" );
  fprintf( stderr, " -g            ignore quotes\n" );
  fprintf( stderr, " -h            this info\n" );
  fprintf( stderr, " -l {line}     show all lines that match template of {line}\n");
  fprintf( stderr, " -L {fname}    show all the lines that match templates of lines in {fname}\n");
  fprintf( stderr, " -m {template} show all lines that match {template}\n");
  fprintf( stderr, " -M {fname}    show all the lines that match templates in {fname}\n");
  fprintf( stderr, " -n {num}      max cluster args [default: %d]\n", MAX_ARGS_IN_FIELD );
  fprintf( stderrm " -t {file}     load templates to ignore\n" );
  fprintf( stderr, " -v            display version information\n" );
  fprintf( stderr, " -w {file}     save templates to file\n" );
  fprintf( stderr, " filename      one or more files to process, use '-' to read from stdin\n" );
#endif

  fprintf( stderr, "\n" );
}

/****
 *
 * cleanup
 *
 ****/

PRIVATE void cleanup( void ) {
  /* free any match templates */
  cleanMatchList();

  if ( config->outFile_st != NULL )
    fclose( config->outFile_st );
  XFREE( config->hostname );
#ifdef MEM_DEBUG
  XFREE_ALL();
#else
  XFREE( config );
#endif
}

/*****
 *
 * interrupt handler (current time)
 *
 *****/

void ctime_prog( int signo ) {
  time_t ret;

  /* disable SIGALRM */
  signal( SIGALRM, SIG_IGN );
  /* update current time */
  reload = TRUE;

  /* reset SIGALRM */
  signal( SIGALRM, ctime_prog );
  /* reset alarm */
  alarm( ALARM_TIMER );
}
