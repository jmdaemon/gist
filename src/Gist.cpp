#include "Data.h"
#include "Version.h"
#include "Str.h"
#include "Api.h"

#include <algorithm>
#include <cstdlib> /* getenv */
#include <fstream>
#include <istream>
#include <tuple>

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
  std::string deleteID, id, searchID, created_at, updated_at;
  bool listAllGists{false}, createNewGist{false}, isPriv{false}, raw{false};
  std::string searchGists{""}, hasDesc{""}, hasName{""};
  std::tuple<std::string, std::string> gist{ "", "" };
};

int cleanup() {
  RestClient::disable();
  return 0;
}

std::string substituteHyphenColon(std::string str) {
  return substitute(str, "-", ":");
}

void search(json& o, Options options) {
    if (!options.raw) {
      fmt::print("{}\n", o.dump(4));
      return;
    }
    std::for_each(o["files"].begin(), o["files"].end(), [] (json& gist) { fmt::print("{}\n", unquote(gist["raw_url"])); });
}

int searchDate(json o, Options options, std::string date, std::string gistDate) {
  std::for_each(o.begin(), o.end(), std::bind([] (json o, Options options, std::string date, std::string gistDate) {
    std::string gist_date  = substituteHyphenColon(gistDate);
    if (gist_date > date) { search(o, options); } // Retrieve gists later than date
    } , std::placeholders::_1, options, date, gistDate));
  return cleanup();
}

inline int searchID(json o, Options options) {
  std::for_each(o.begin(), o.end(), std::bind([] (json o, Options options) {
    if (o["id"] == options.searchID) { search(o, options); } return;
    } , std::placeholders::_1, options));
  return cleanup();
}

inline int searchFile(json o, Options options) {
  std::for_each(o.begin(), o.end(), std::bind([] (json o, Options options) {
        for (auto& file : o["files"]) { if (file["filename"] == options.hasName) search(o, options); }
        }, std::placeholders::_1, options));
  return cleanup();
}

struct App {
  CLI::App* app;
  Options* options;
};

//auto buildCli(std::string GIST_CONFIG) {
App buildCli(std::string GIST_CONFIG) {
  CLI::App app{"Manage your GitHub gists"};
  app.allow_extras();
  app.formatter(std::make_shared<ArgFormat>());
  app.get_formatter()->column_width(40);

  auto showVersion = [](int) { fmt::print("gist v{}.{}.{}\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH); };

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
  search->add_flag("-r, --raw", options.raw, "Output raw url of gist");
  search->add_option("-p", options.isPriv, "Search only private gists");
  search->add_option("-i, --id", options.searchID, "Search by gist id");
  search->add_option("-f, --filename", options.hasName, "Search by gist filename");
  search->add_option("-c, --creation-date", options.created_at, "Search by date created");
  search->add_option("-d, --date", options.updated_at, "Search by last modified date");

  showVersionCmd->excludes("-l", "-D", "-u", "-n");
  listCmd       ->excludes("-v", "--version", "-D", "-u", "-n");
  deleteCmd     ->excludes("-v", "--version", "-l", "-u", "-n");
  updateCmd     ->excludes("-v", "--version", "-l", "-D", "-n");
  createNewGist ->excludes("-v", "--version", "-l", "-D", "-u");
  //return std::make_tuple(app, options);
  auto result = App { &app, &options };
  return result;
}

int main(int argc, char** argv) { 
  std::string GIST_CONFIG = std::string(std::getenv("HOME")) + "/.config/gist/config.toml"; 
  auto cli = buildCli(GIST_CONFIG);
  auto [ app, options ] = cli;
  //CLI::App app;
  //Options options;
  //(app, options) = buildCli(GIST_CONFIG);
  //App app = buildCli();
  //std::tie(app, options) = buildCli(GIST_CONFIG);

  CLI11_PARSE(*app, argc, argv); 
  auto config = parseConfig(GIST_CONFIG);
  RestClient::init();

  if (options->listAllGists) {
    fmt::print("{}\n", (getGists(config).dump(4)));
    return cleanup();
  }

  if (*search) {
    auto o = getGists(config);

    if (!options->searchID.empty()) {
      return searchID(o, *options);
    }

    if (!options->hasName.empty()) {
      return searchFile(o, *options);
    }

    if (!options->created_at.empty()) {
      return searchDate(o, *options, substituteHyphenColon(options->created_at + "T00:00:00Z"), "created_at");
    }

    if(!options->updated_at.empty()) {
      return searchDate(o, *options, substituteHyphenColon(options->updated_at + "T00:00:00Z"), "updated_at");
    }
    return cleanup();
  }

  if (!options->id.empty()) {
    std::string id = parseID(options->id);
    std::string fname = (!app->remaining()[0].empty()) ? app->remaining()[0]: "gistFile1.txt";
    //Data data = {id, fname, options.hasDesc, readFile(fname), options.isPriv};
    Data data = {id, fname, options->hasDesc, read_file(fname.c_str()), options->isPriv};
    //printData(data);
    updateGist(data, config);
    return cleanup();
  }

  if (!options->deleteID.empty()) {
    send("DELETE", config, Data{options->id});
    return cleanup();
  }

  // Create new gist from STDIN
  if (options->createNewGist) {
    Data data = Data{options->id, readInput("Gist file name"), readInput("Gist description"), readInput("Gist contents")};
    printResponse(send("POST", config, data));
    return cleanup();
  }

  // Create new gist from files
  for (auto& gist : app->remaining()) {
    //Data data = {options.id, gist, options.hasDesc, readFile(gist), options.isPriv};
    Data data = {options->id, gist, options->hasDesc, read_file(gist.c_str()), options->isPriv};
    printResponse(send("POST", config, data));
  }
  return cleanup();
}
