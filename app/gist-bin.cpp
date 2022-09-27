#include "gist.h"

USE_JOLLY_SPDLOG_SETUP_LOGGER_FUNCTION
USE_JOLLY_SPDLOG_SETUP_LOGGING_FUNCTION

int main(int argc, char** argv) {
  // Parse arguments
  struct arguments arguments = set_default_args();
  argp_parse (&argp, argc, argv, 0, 0, &arguments);
  const char* command = arguments.args[0];

  // Setup library logging
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
  auto logger = setup_logger(sinks);
  setup_logging(arguments.verbose);

  // Set gist config file path
  std::string cfg = GIST_CONFIG_PATH;
  if (arguments.config != nullptr)
    cfg = arguments.config;
  else if (getenv("GIST_CONFIG_HOME") != nullptr)
    cfg = getenv("GIST_CONFIG_HOME");

  auto [token, uname] = parse_config(cfg);

  RestClient::init();
  auto headers = create_headers(token);

  switch(hash(command)) {
    case hash("list"):    list_gists(headers); break;
    case hash("show"):    show_gist(arguments, headers); break;
    case hash("new"):     new_gist(arguments, headers); break;
    case hash("create"):  create_gist(arguments, headers); break;
    case hash("delete"):  delete_gist(arguments, headers); break;
    case hash("update"):  update_gist(arguments, headers); break;
    case hash("search"):  search_gist(arguments, headers); break;
  }

  RestClient::disable();
  return 0;
}
