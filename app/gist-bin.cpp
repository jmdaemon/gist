#include "gist.h"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

//void show_response(nlohmann::json res, bool raw) {
  ////(raw) ? fmt::print("{}", res.body) : fmt::print();
  //// Raw file dump 
  ////if (raw) 
//}

/** Show all gists found for the user */
void list_gists() {
  RestClient::Connection* con = connect(GITHUB_API_URL);
  std::string query = "/gists";
  auto res = get_json(con, query);
  fmt::print("{}", res);
  //show_response(res, args.raw);
}

int main(int argc, char** argv) {
  // Parse arguments
  struct arguments arguments = set_default_args();
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  // TODO: Handle environment variable for gist config
  const char* command = arguments.args[0];

  // TODO: Parse and setup config
  RestClient::init();

  switch(hash(command)) {
    case hash("list"):
      // List every gist available that the user has
      list_gists();
      break;
    case hash("new"): break;
    case hash("delete"): break;
    case hash("update"): break;
    case hash("search"): break;
  }

  RestClient::disable();
  return 0;
}
