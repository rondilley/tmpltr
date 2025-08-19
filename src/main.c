/*****
 *
 * Description: Main Functions
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
 *.
 ****/

#include "main.h"
#include "string_intern.h"
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

PUBLIC int quit = FALSE;
PUBLIC int reload = FALSE;
PUBLIC Config_t *config = NULL;

/****
 *
 * external variables
 *
 ****/


/****
 * secure integer parsing with validation
 ****/
PRIVATE int safe_parse_int(const char *str, int min_val, int max_val, int *result)
{
  char *endptr;
  long val;
  
  if (!str || !result) {
    return FALSE;
  }
  
  errno = 0;
  val = strtol(str, &endptr, 10);
  
  /* Check for conversion errors */
  if (errno == ERANGE || errno == EINVAL) {
    return FALSE;
  }
  
  /* Check if entire string was consumed */
  if (*endptr != '\0') {
    return FALSE;
  }
  
  /* Check bounds */
  if (val < min_val || val > max_val) {
    return FALSE;
  }
  
  *result = (int)val;
  return TRUE;
}

/****
 * validate file path for security
 ****/
PRIVATE int validate_file_path(const char *path)
{
  char *resolved_path;
  
  if (!path) {
    return FALSE;
  }
  
  /* Check path length */
  if (strlen(path) >= PATH_MAX) {
    fprintf(stderr, "ERR - Path too long: %s\n", path);
    return FALSE;
  }
  
  /* Allow stdin */
  if (strcmp(path, "-") == 0) {
    return TRUE;
  }
  
  /* Resolve the path to detect traversal attempts */
  resolved_path = realpath(path, NULL);
  if (resolved_path == NULL) {
    /* If realpath fails, the file might not exist yet, which is okay for some operations */
    /* But we still need to check for obvious traversal patterns */
    if (strstr(path, "../") != NULL && strstr(path, "../") != path) {
      /* Allow relative paths that start with ../ but reject embedded ../ */
      const char *ptr = path;
      while (strncmp(ptr, "../", 3) == 0) {
        ptr += 3;
      }
      if (strstr(ptr, "../") != NULL) {
        fprintf(stderr, "ERR - Path traversal detected in: %s\n", path);
        return FALSE;
      }
    }
    return TRUE;  /* Allow non-existent files for write operations */
  }
  
  /* Check for absolute paths to sensitive directories */
  if (strncmp(resolved_path, "/etc/", 5) == 0 ||
      strncmp(resolved_path, "/proc/", 6) == 0 ||
      strncmp(resolved_path, "/sys/", 5) == 0 ||
      strncmp(resolved_path, "/dev/", 5) == 0 ||
      strncmp(resolved_path, "/root/", 6) == 0) {
    fprintf(stderr, "ERR - Access to system directory denied: %s\n", resolved_path);
    free(resolved_path);
    return FALSE;
  }
  
  free(resolved_path);
  return TRUE;
}

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
 * main function
 *
 ****/

int main(int argc, char *argv[])
{
  PRIVATE int c = 0;

#ifndef DEBUG
  struct rlimit rlim;

  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);
#endif

  /* setup config */
  config = (Config_t *)XMALLOC(sizeof(Config_t));
  XMEMSET(config, 0, sizeof(Config_t));

  /* force mode to forground */
  config->mode = MODE_INTERACTIVE;

  /* default parser */
  config->parser_type = PARSER_TYPE_LEGACY;

  /* store current pid */
  config->cur_pid = getpid();

  /* get real uid and gid in prep for priv drop */
  config->gid = getgid();
  config->uid = getuid();

  while (1)
  {
#ifdef HAVE_GETOPT_LONG
    int option_index = 0;
    static struct option long_options[] = {
        {"cluster", no_argument, 0, 'c'},
        {"greedy", no_argument, 0, 'g'},
        {"version", no_argument, 0, 'v'},
        {"debug", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {"cnum", required_argument, 0, 'n'},
        {"templates", required_argument, 0, 't'},
        {"write", required_argument, 0, 'w'},
        {"match", required_argument, 0, 'm'},
        {"matchfile", required_argument, 0, 'M'},
        {"line", required_argument, 0, 'l'},
        {"linefile", required_argument, 0, 'L'},
        {"quiet", no_argument, 0, 'q'},
        {"no-output", no_argument, 0, 'q'},
        {0, no_argument, 0, 0}};
    c = getopt_long(argc, argv, "vd:hn:t:w:cCgm:M:l:L:q", long_options, &option_index);
#else
    c = getopt(argc, argv, "vd:htn::w:cgm:M:l:L:q");
#endif

    if (c == -1)
      break;

    switch (c)
    {

    case 'v':
      /* show the version */
      print_version();
      return (EXIT_SUCCESS);

    case 'c':
      /* enable argument clustering */
      config->cluster = TRUE;
      break;

    case 'd':
      /* show debug info */
      if (!safe_parse_int(optarg, 0, 9, &config->debug)) {
        fprintf(stderr, "ERR - Invalid debug level: %s (must be 0-9)\n", optarg);
        return (EXIT_FAILURE);
      }
      break;

    case 'g':
      /* ignore quotes */
      config->greedy = TRUE;
      break;

    case 'n':
      /* override default cluster count */
      if (!safe_parse_int(optarg, 1, 10000, &config->clusterDepth)) {
        fprintf(stderr, "ERR - Invalid cluster depth: %s (must be 1-10000)\n", optarg);
        return (EXIT_FAILURE);
      }
      break;

    case 't':
      /* load template file */
      if (!validate_file_path(optarg)) {
        return (EXIT_FAILURE);
      }
      if (loadTemplateFile(optarg) != TRUE)
      {
        fprintf(stderr, "ERR - Problem while loading template file\n");
        return (EXIT_FAILURE);
      }
      break;

    case 'h':
      /* show help info */
      print_help();
      return (EXIT_SUCCESS);

    case 'w':
      /* save templates to file */
      if (!validate_file_path(optarg)) {
        return (EXIT_FAILURE);
      }
      if ((config->outFile_st = secure_fopen(optarg, "w")) == NULL)
      {
        fprintf(stderr, "ERR - Unable to open template file for write [%s]\n", optarg);
        return (EXIT_FAILURE);
      }
      break;

    case 'M':
      /* load match templates from file */
      if (!validate_file_path(optarg)) {
        return (EXIT_FAILURE);
      }
      config->match = loadMatchTemplates(optarg);
      break;

    case 'm':
      /* add template to match list */
      config->match = addMatchTemplate(optarg);
      break;

    case 'L':
      /* load match lines from file and convert to templates */
      if (!validate_file_path(optarg)) {
        return (EXIT_FAILURE);
      }
      config->match = loadMatchLines(optarg);
      break;

    case 'l':
      /* convert match line and add as template */
      config->match = addMatchLine(optarg);
      break;

    case 'q':
      /* enable quiet mode - skip printing templates at end */
      config->no_output = TRUE;
      break;

    default:
      fprintf(stderr, "Unknown option code [0%o]\n", c);
    }
  }

  /* override cluster depth */
  if ((config->clusterDepth <= 0) || (config->clusterDepth > 10000))
    config->clusterDepth = MAX_ARGS_IN_FIELD;

  /* check dirs and files for danger */

  if (time(&config->current_time) == -1)
  {
    display(LOG_ERR, "Unable to get current time");

    /* cleanup buffers */
    cleanup();
    return (EXIT_FAILURE);
  }

  /* initialize program wide config options */
  config->hostname = (char *)XMALLOC(MAXHOSTNAMELEN + 1);

  /* get processor hostname */
  if (gethostname(config->hostname, MAXHOSTNAMELEN) != 0)
  {
    display(LOG_ERR, "Unable to get hostname");
    strcpy(config->hostname, "unknown");
  }

  config->cur_pid = getpid();

  /* setup current time updater */
  signal(SIGALRM, ctime_prog);
  alarm(ALARM_TIMER);

  /*
   * get to work
   */

  /* process all the files */
  while (optind < argc)
  {
    if (!validate_file_path(argv[optind])) {
      fprintf(stderr, "ERR - Invalid file path: %s\n", argv[optind]);
      cleanup();
      return (EXIT_FAILURE);
    }
    processFile(argv[optind++]);
  }

  if (config->match)
  {
    /* XXX should print match metrict */
  }
  else
  {
    /* print the templates we have found */
    if (!config->no_output) {
      showTemplates();
    } else {
      fprintf(stderr, "Template processing complete. Use without -q to see results.\n");
    }
  }

  /*
   * finished with the work
   */

  cleanup();

  return (EXIT_SUCCESS);
}

/*****
 *
 * display version info
 *
 *****/

PRIVATE void print_version(void)
{
  printf("%s v%s [%s - %s]\n", PROGNAME, VERSION, __DATE__, __TIME__);
}

/*****
 *
 * print help info
 *
 *****/

PRIVATE void print_help(void)
{
  print_version();

  fprintf(stderr, "\n");
  fprintf(stderr, "syntax: %s [options] filename [filename ...]\n", PACKAGE);

#ifdef HAVE_GETOPT_LONG
  fprintf(stderr, " -c|--cluster           show invariable fields in output\n");
  fprintf(stderr, " -d|--debug (0-9)       enable debugging info\n");
  fprintf(stderr, " -g|--greedy            ignore quotes\n");
  fprintf(stderr, " -h|--help              this info\n");
  fprintf(stderr, " -l|--line {line}       show all lines that match template of {line}\n");
  fprintf(stderr, " -L|--linefile {fname}  show all the lines that match templates of lines in {fname}\n");
  fprintf(stderr, " -m|--match {template}  show all lines that match {template}\n");
  fprintf(stderr, " -M|--matchfile {fname} show all the lines that match templates in {fname}\n");
  fprintf(stderr, " -n|--cnum {num}        max cluster args [default: %d]\n", MAX_ARGS_IN_FIELD);
  fprintf(stderr, " -t|--templates {file}  load templates to ignore\n");
  fprintf(stderr, " -v|--version           display version information\n");
  fprintf(stderr, " -w|--write {file}      save templates to file\n");
  fprintf(stderr, " filename               one or more files to process, use '-' to read from stdin\n");
#else
  fprintf(stderr, " -c            show invariable fields in output\n");
  fprintf(stderr, " -d {lvl}      enable debugging info\n");
  fprintf(stderr, " -g            ignore quotes\n");
  fprintf(stderr, " -h            this info\n");
  fprintf(stderr, " -l {line}     show all lines that match template of {line}\n");
  fprintf(stderr, " -L {fname}    show all the lines that match templates of lines in {fname}\n");
  fprintf(stderr, " -m {template} show all lines that match {template}\n");
  fprintf(stderr, " -M {fname}    show all the lines that match templates in {fname}\n");
  fprintf(stderr, " -n {num}      max cluster args [default: %d]\n", MAX_ARGS_IN_FIELD);
  fprintf(stderr, " -t {file}     load templates to ignore\n");
  fprintf(stderr, " -v            display version information\n");
  fprintf(stderr, " -w {file}     save templates to file\n");
  fprintf(stderr, " filename      one or more files to process, use '-' to read from stdin\n");
#endif

  fprintf(stderr, "\n");
}

/****
 *
 * cleanup
 *
 ****/

PRIVATE void cleanup(void)
{
  /* free any match templates */
  cleanMatchList();
  
  /* cleanup global string interning system */
  cleanupGlobalIntern();

  if (config->outFile_st != NULL)
    fclose(config->outFile_st);
  XFREE(config->hostname);
#ifdef MEM_DEBUG
  XFREE_ALL();
#else
  XFREE(config);
#endif
}

/*****
 *
 * interrupt handler (current time)
 *
 *****/

void ctime_prog(int signo)
{
  /* disable SIGALRM */
  signal(SIGALRM, SIG_IGN);
  /* update current time */

  if (signo != SIGALRM)
  {
    fprintf(stderr, "ERR - ctime_prog called with unexpected signal %d\n", signo);
    return;
  }

  if (time(&config->current_time) == -1)
    fprintf(stderr, "ERR - Unable to update current time\n");
  config->alarm_count++;
  if (config->alarm_count == 60)
  {
    reload = TRUE;
    config->alarm_count = 0;
  }

  /* reset SIGALRM */
  signal(SIGALRM, ctime_prog);
  /* reset alarm */
  alarm(ALARM_TIMER);
}
