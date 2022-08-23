#include "gist_def.h"
#include "cli.h"
#include <jolly.h>

int main(int argc, char** argv) {

  // Parse arguments
  struct arguments arguments = set_default_args();
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  // Handle environment variable for gist config
  
  char* command = arguments.args[0];

  /*
  switch(shash(command)) {
    case shash("new"): break;
    case shash("list"): break;
    case shash("delete"): break;
    case shash("update"): break;
    case shash("search"): break;
  }
  */

  return 0;
}
