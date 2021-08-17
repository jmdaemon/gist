#include "Data.h"
#include "Version.h"

#include <algorithm>
#include <cstdlib> /* getenv */
#include <fstream>
#include <istream>
#include <tuple>
//#include <time.h> [> difftime <]
//#include <chrono>

// Libraries
#include <toml++/toml.h>
#include <nlohmann/json.hpp>
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
//#include <argparse/argparse.hpp>

using nlohmann::json;

void printResponse(RestClient::Response res) {
  std::string response = json::parse(res.body).dump(4);
  fmt::print("{}\n", response);
}

std::string createJson(Data data) {
  json o;
  o["description"]                  = data.desc;
  o["public"]                       = data.pub;
  o["files"][data.fname]["content"] = data.contents;
  return o.dump();
}

auto createHeaders(std::optional<std::string> token) {
  RestClient::HeaderFields headers;
  headers["Accept"] = "application/vnd.github.v3+json";
  headers["Authorization"] = "token " + *token;
  return headers;
}

RestClient::Connection* connect(std::string url, std::optional<std::string> token) {
  RestClient::Connection* conn = new RestClient::Connection(url);
  conn->FollowRedirects(true);
  conn->SetHeaders(createHeaders(token)); 
  return conn;
}

struct Config { std::string url, username, token; };

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

auto send(std::string cmd, Config config, Data data) {
  auto conn = connect(config.url, config.token);
  switch(hash(cmd.c_str())) {
    case hash("GET")    : return conn->get("/gists");
    case hash("POST")   : return conn->post("/gists", createJson(data));
    case hash("DELETE") : return conn->del("/gists/" + data.id);
    case hash("PATCH")  : return conn->patch("/gists/" + data.id, createJson(data));
    default             : return RestClient::Response();
  }
}

json getGists(Config& config) {
  config.url = config.url + "/users/" + config.username;
  return json::parse(send("GET", config, {}).body);
}

void updateGist(Data& data, Config& config) {
  printResponse(send("PATCH", config, data));
}

static std::string readFile(const std::filesystem::path& path) {
    if (!std::filesystem::is_regular_file(path))
        return { };

    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
        return { };

    const std::size_t& size = std::filesystem::file_size(path);
    std::string content(size, '\0');
    file.read(content.data(), size);
    file.close();
    return content;
}

class ArgFormat : public CLI::Formatter {
  public:
    // Remove text between option name and description
    std::string make_option_opts(const CLI::Option *) const override {return "";}
};

// Parse gist IDs & gist urls
auto parseID(std::string str, char delim = '/') -> std::string {
  std::istringstream ss(str);
  std::string token;
  std::vector<std::string> tokens; 
  while(std::getline(ss, token, delim))
    tokens.push_back(token); 
  std::string result = std::string(tokens.back());
  return result;
}

// Parse auth token
auto parseConfig(std::string GIST_CONFIG) {
  const std::string url = "https://api.github.com";
  auto config = toml::parse_file(GIST_CONFIG);
  const std::optional<std::string> token    = config["user"]["token"].value<std::string>();
  const std::optional<std::string> username = config["user"]["name"].value<std::string>();

  // Enable on verbose mode
  //fmt::print("\n==== Config ====\n");
  //fmt::print("Token    : [{}]\n", *token);
  //fmt::print("Username : [{}]\n\n", *username);

  if (token->empty()) {
    fmt::print("Token must not be empty");
    throw std::exception();
  } else if (username->empty()) {
    fmt::print("Username must not be empty");
    throw std::exception();
  }
  return Config{url, *username, *token};
}

struct Options {
  std::string deleteID, id, searchID, created_at, updated_at, regex;
  bool listAllGists{false}, createNewGist{false}, isPriv{false};
  std::string searchGists{""}, hasDesc{""}, hasName{""};
  std::tuple<std::string, std::string> gist{ "", "" };
};

int cleanup() {
  RestClient::disable();
  return 0;
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

int searchDate(json o, Options options) {
  for (int i = 0; i < o.size(); i++) {
    std::string updated_at = replaceAll(options.updated_at + "T00:00:00Z", "-", ":");
    std::string gist_date  = replaceAll(o[i]["updated_at"], "-", ":");
    if (gist_date > updated_at) { // Retrieve gists later than created_at
      fmt::print("{}\n", o[i].dump(4));
    }
  }
  return cleanup();
}

int searchID(json o, Options options) {
  for (int i = 0; i < o.size(); i++) {
    if (o[i]["id"] == options.searchID) {
      fmt::print("{}\n", o[i].dump(4));
      return cleanup();
    }
  }
  return cleanup();
}

int searchFile(json o, Options options) {
  for (int i = 0; i < o.size(); i++) {
    std::string result((*o[i]["files"].begin())["filename"]);
    if (result == options.hasName) {
      fmt::print("{}\n", o[i].dump(4));
      return cleanup();
    }
  }
  return cleanup();
}

int main(int argc, char** argv) { 
  CLI::App app{"Manage your GitHub gists"};
  app.allow_extras();
  app.formatter(std::make_shared<ArgFormat>());
  app.get_formatter()->column_width(40);

  auto showVersion = [](int) { fmt::print("gist v{}.{}.{}\n", GIST_VERSION_MAJOR, GIST_VERSION_MINOR, GIST_VERSION_PATCH); };
  std::string GIST_CONFIG = std::string(std::getenv("HOME")) + "/.config/gist/config.toml"; 

  Options options;
  app.add_option("--config", GIST_CONFIG, "Specify gist config for user")->envname("GIST_CONFIG_HOME");
  app.add_option("-p", options.isPriv, "Makes your gist private");
  app.add_option("-d", options.hasDesc, "Adds a description to your gist");
  app.add_option("-f", options.hasName, "Sets the filename and syntax type");
  
  auto showVersionCmd = app.add_flag_function("-v,--version", showVersion, "Show gist cli version");
  auto listCmd = app.add_flag("-l"      , options.listAllGists  , "Lists all user gists");
  auto deleteCmd = app.add_option("-D"  , options.deleteID      , "Delete a gist");
  auto updateCmd = app.add_option("-u"  , options.id            , "Update an existing gist");
  auto createNewGist = app.add_flag("-n", options.createNewGist , "Make a new gist from STDIN");
  auto search = app.add_subcommand("search", "Search user gists");
  search->add_option("-p", options.isPriv, "Search only private gists");
  search->add_option("-i, --id", options.searchID, "Search by gist id");
  search->add_option("-f, --filename", options.hasName, "Search by gist filename");
  search->add_option("-c, --creation-date", options.created_at, "Search by date created");
  search->add_option("-d, --date", options.updated_at, "Search by last modified date");
  search->add_option("-r, --regex", options.regex, "Search by regex");

  showVersionCmd->excludes("-l", "-D", "-u", "-n");
  listCmd       ->excludes("-v", "--version", "-D", "-u", "-n");
  deleteCmd     ->excludes("-v", "--version", "-l", "-u", "-n");
  updateCmd     ->excludes("-v", "--version", "-l", "-D", "-n");
  createNewGist ->excludes("-v", "--version", "-l", "-D", "-u");

  CLI11_PARSE(app, argc, argv); 
  auto config = parseConfig(GIST_CONFIG);
  RestClient::init();

  if (options.listAllGists) {
    fmt::print("{}\n", (getGists(config).dump(4)));
    return cleanup();
  }

  //if (!options.searchGists.empty()) {
  if (*search) {
    auto o = getGists(config);

    if (!options.searchID.empty()) {

      //std::for_each(o.begin(), o.end(), std::bind(
            //[] (json o, Options options) {
            //if (o["id"] == options.searchID) {
              //fmt::print("{}\n", o.dump(4));
              //return;
            //}} , std::placeholders::_1, options));
      //return cleanup();
      return searchID(o, options);
    }

    if (!options.hasName.empty()) {
      return searchFile(o, options);
    }

    if (!options.created_at.empty() || !options.updated_at.empty()) {
      return searchDate(o, options);
    }
    return cleanup();
  }

  if (!options.id.empty()) {
    std::string id = parseID(options.id);
    std::string fname = app.remaining()[0].empty() ? app.remaining()[0]: "gistFile1.txt";
    //std::string fname = getFilename(getGists(config), id);
    //Data data = {options.id, getFilename(getGists(config), id), options.hasDesc, readInput(), options.isPriv};
    Data data = {id, fname, options.hasDesc, readFile(fname), options.isPriv};
    updateGist(data, config);
    return cleanup();
  }

  if (!options.deleteID.empty()) {
    send("DELETE", config, Data{options.id});
    return cleanup();
  }

  // Create new gist from STDIN
  if (options.createNewGist) {
    Data data = Data{options.id, readInput(), readInput(), readInput()};
    printResponse(send("POST", config, data));
    return cleanup();
  } 

  // Create new gist from files
  for (auto& gist : app.remaining()) {
    Data data = {options.id, gist, options.hasDesc, readFile(gist), options.isPriv};
    printResponse(send("POST", config, data));
  }
  return cleanup();
}
