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

  // Set gist config path
  std::string cfg = GIST_CONFIG_PATH;
  if (arguments.config != nullptr)
    cfg = arguments.config;
  else if (getenv("GIST_CONFIG_HOME") != nullptr)
    cfg = getenv("GIST_CONFIG_HOME");

  // Parse config
  auto [token, uname] = parse_config(cfg);

  // Create headers
  auto headers = create_headers(token);

  RestClient::init();

  switch(hash(command)) {
    case hash("list"): {
      // List every gist available that the user has
      //list_gists();
      RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
      //std::string query = fmt::format("{}/gists/users/{}", GITHUB_API_URL, uname);
      std::string query = fmt::format("/gists");
      //auto res = get_json(con, query);
      //fmt::print("{}", res.dump(INDENT_LEVEL));
      auto res = send_req(con, "GET", query);

      SPDLOG_DEBUG("Response Code: {}", res.code);
      //SPDLOG_DEBUG("Response Headers:\n{}", res.headers);
      SPDLOG_DEBUG("Response Body:\n{}", res.body);

      auto json_res = nlohmann::json::parse(res.body);
      fmt::print("{}", json_res.dump(INDENT_LEVEL));
      break;
    }
    case hash("new"): break;
    case hash("delete"): break;
    case hash("update"): break;
    case hash("search"): break;
  }

  RestClient::disable();
  return 0;
}
