#include "cli.h"
#include "Version.h"

struct arguments set_default_args() {
  struct arguments arguments;

  /* Default values. */
  arguments.verbose = 0;
  arguments.priv = 0;
  arguments.raw = 0;
  return arguments;
}

error_t parse_opt (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;
  switch (key) {
    case 'V':
      printf("gist %s v%d.%d.%d\n", email, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
      exit(0);

    case 'v':
      arguments->verbose = 1;
      break;

    case 'p':
      arguments->priv = true;
      break;

    case 'r':
      arguments->raw = true;
      break;

    case 'g':
      arguments->config = arg;
      break;

    case 'd':
      arguments->gist.desc = arg;
      break;

    case 'i':
      arguments->gist.id = arg;
      break;

    case 'f':
      arguments->gist.filename = arg;
      break;

    case 'c':
      arguments->gist.creation = arg;
      break;

    case 'm':
      arguments->gist.modified = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num > 1)
        /* Too many arguments. */
        argp_usage (state);
      arguments->args[state->arg_num] = arg;
      break;

    case ARGP_KEY_END:
      if (state->arg_num < 1) {
        /* Not enough arguments. */
        argp_usage (state);
      }
      break;

    default:
      return ARGP_ERR_UNKNOWN;

    }
  return 0;
}
