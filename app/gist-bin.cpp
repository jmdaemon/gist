#include "gist.h"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

std::shared_ptr<spdlog::logger> setup_logger(std::vector<spdlog::sink_ptr> sinks) {
  auto logger = spdlog::get(logger_name);
  if (!logger) {
    if (sinks.size() > 0) {
        logger = std::make_shared<spdlog::logger>(logger_name, std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);
    } else {
        logger = spdlog::stdout_color_mt(logger_name);
    }
  }
  return logger;
}

/* Enable/disable logging */
void setup_logging(bool verbose) {
  if (verbose == 1)
    spdlog::set_level(spdlog::level::level_enum::trace);
  else {
    // Default to no logging
    spdlog::set_level(spdlog::level::level_enum::off);
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_NONE
  }
  spdlog::cfg::load_env_levels();
  spdlog::set_pattern(logger_format);
}

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
    case hash("new"):     new_gist(arguments, headers); break;
    case hash("create"):  create_gist(arguments, headers); break;
    case hash("delete"):  delete_gist(arguments, headers); break;
    case hash("update"):  update_gist(arguments, headers); break;
    case hash("search"):  search_gist(arguments, headers); break;
  }

  RestClient::disable();
  return 0;
}
