#include "gist.h"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

int main(int argc, char** argv) {
  // Parse arguments
  struct arguments arguments = set_default_args();
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  // Handle environment variable for gist config
  
  const char* command = arguments.args[0];

  RestClient::init();

  switch(hash(command)) {
    case hash("list"): break;
    case hash("new"): break;
    case hash("delete"): break;
    case hash("update"): break;
    case hash("search"): break;
  }

  RestClient::disable();
  return 0;
}
