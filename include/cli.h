#pragma once
#ifndef GIST_CLI_H
#define GIST_CLI_H

/* Support header include for C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Imports */
#include "gist.h"

/* Standard Library */
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** Author and maintainer's email. Contact me directly here for questions or bug reports, or use the issues page. */
static const char *email = "<josephm.diza@gmail.com>";

/** Program summary. */
static const char doc[] = "Gist - Manage your GitHub gists";

/** A description of the arguments we accept. */
static const char args_doc[] = "[list/delete/update/new/search] [options]";

/** http://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html <br> */
/** The options we understand. */
static const struct argp_option options[] = {
  {"version"  , 'V', 0, 0, "Show program version", 0},
  {"verbose"  , 'v', 0, 0, "Display program output and debugging information", 0},

  {"private"  , 'p', 0, 0, "Makes your gist private", 0},
  {"raw"      , 'r', "Output raw url of gist", 0},

  {"config"   , 'g', "Specify gist config for user", 0},
  {"desc"     , 'd', "Adds a description to your gist", 0},

  // Search flags
  {"id"       , 'i', "Specify gist id", 0},
  {"filename" , 'f', "Specify gist file name", 0},
  {"creation" , 'c', "Specify gist creation date", 0},
  {"modified" , 'm', "Specify gist modification date", 0},
  {}
};

/** Used by main to communicate with parse_opt. */
struct arguments {
  char *args[1]; /** Commands: list, delete, update, new, search */
  int verbose;
  bool priv;
  bool raw;
  char* config;
  Gist gist;
};

/** Parse a single option. */
error_t parse_opt (int key, char *arg, struct argp_state *state);

/** http://www.gnu.org/software/libc/manual/html_node/Argp-Parsers.html <br> */
/** http://www.gnu.org/software/libc/manual/html_node/Argp-Help-Filtering.html <br> */
/** Our argp parser. */
static const struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0};

/** Set default values for arguments */
struct arguments set_default_args();

#ifdef __cplusplus
}
#endif
#endif // GIST_CLI_H
