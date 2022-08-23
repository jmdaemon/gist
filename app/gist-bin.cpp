#include "gist.h"
#include <cstdio>

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

int main(int argc, char** argv) {
  // Parse arguments
  struct arguments arguments = set_default_args();
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  const char* command = arguments.args[0];

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
      std::string query = fmt::format("/gists/users/{}", uname);
      auto res = get_json(con, query);
      fmt::print("{}", res);
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
