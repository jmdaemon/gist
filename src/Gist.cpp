#include "Data.h"
#include "Version.h"

#include <cstdlib> /* getenv */
#include <fstream>

// Libraries
#include <toml++/toml.h>
#include <nlohmann/json.hpp>
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include <argparse/argparse.hpp>

using nlohmann::json;

void showUsage() {
  fmt::print("Usage: gist [filename]");
  fmt::print("      -h, --help                    Show this message and exit\n");
  fmt::print("      -v, --version,                Show gist version\n");
  fmt::print("      -u id [id],                   Update an existing gist specified by id\n");
  fmt::print("      -u fname [fname],             Update an existing gist specified by filename\n");
  fmt::print("      -n [fname] [desc] [contents], Create a new gist from user input prompts for gist\n");
  fmt::print("      -D [id],                      Delete a gist\n\n");
}

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

RestClient::Connection* createConnection(std::string url, std::optional<std::string> token) {
  RestClient::Connection* conn = new RestClient::Connection(url);
  conn->FollowRedirects(true);
  conn->SetHeaders(createHeaders(token)); 
  return conn;
}

auto sendGET(RestClient::Connection* conn) { return conn->get("/gists"); }
auto sendPATCH(RestClient::Connection* conn, std::string gistID, std::string contents) { return conn->patch("/gists/" + gistID, contents); }
auto sendDELETE(RestClient::Connection* conn, std::string gistID) { return conn->del("/gists/" + gistID); }
auto createGist(RestClient::Connection* conn, std::string contents) { return conn->post("/gists", contents); }

json listGists(std::string url, std::string username, std::optional<std::string> token) {
  auto conn = createConnection(url + "/users/" + username, token);
  auto o = json::parse(sendGET(conn).body);
  return o;
}

void updateGist(Data data, std::string url, std::optional<std::string> token, std::string msg) {
  fmt::print("{}\n", msg);
  printData(data);
  auto contents = createJson(data);
  auto conn     = createConnection(url, token);
  auto res      = sendPATCH(conn, data.id, contents);
  printResponse(res);
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

class MyFormatter : public CLI::Formatter {
  public:
    // Remove text between option name and description
    std::string make_option_opts(const CLI::Option *) const override {return "";}
};

int main(int argc, char** argv) { 
  CLI::App app{"Manage your GitHub gists"};

  app.allow_extras();

  auto showVersion = [](int) { fmt::print("gist v{}.{}.{}\n", GIST_VERSION_MAJOR, GIST_VERSION_MINOR, GIST_VERSION_PATCH); };
  app.add_flag_function("-v,--version", showVersion, "Show gist version");

  std::string GIST_CONFIG = std::string(std::getenv("HOME")) + "/.config/gist/config.toml"; 
  app.add_option("--config", GIST_CONFIG, "Specify gist config")->envname("GIST_CONFIG_HOME");

  app.formatter(std::make_shared<MyFormatter>());
  app.get_formatter()->column_width(40);

  auto listCmd = app.add_flag("-l", "List all user gists");
  app.excludes(listCmd);

  std::string id; 

  auto deleteCmd = app.add_option("-D", id, "Delete a gist");
  app.excludes(deleteCmd);

  //auto updateID = app.add_option("-u", id, "Update an existing gist");
  //app.excludes(updateID);

  //auto getExistingGist = [](std::string) { fmt::print(""); } ;
  //app.add_flag_function("-u", getExistingGist, "Update an existing gist");

  //std::string fname;
  //auto updateFname = app.add_option("-u fname", fname, "Update an existing gist");
  //app.excludes(updateFname);
  
  bool newGist{false};
  app.add_flag("-n", newGist, "Make a new gist from STDIN");

  CLI11_PARSE(app, argc, argv); 

  // Parse auth token
  auto config = toml::parse_file(GIST_CONFIG);
  const std::optional<std::string> token      = config["user"]["token"].value<std::string>();
  const std::optional<std::string> username   = config["user"]["name"].value<std::string>();
  const std::string url = "https://api.github.com";

  fmt::print("\n==== Config ====\n");
  fmt::print("Token    : [{}]\n", *token);
  fmt::print("Username : [{}]\n\n", *username);

  RestClient::init();

  if (listCmd) {
    fmt::print("Listing all gists for {}\n", *username);
    listGists(url, *username, token);
  //} else if (!id.empty() || !fname.empty()) { 
  } else if (!id.empty()) { 
    auto o = listGists(url, *username, token);
    Data data;

    if (!id.empty())
      data = {id, getFilename(o, id), readInput(), readInput()};
    //else if (!fname.empty())
      //data = Data{getId(o, fname), fname, readInput(), readInput()};

    updateGist(data, url, token, fmt::format(fmt::runtime("Updating gist {} for {}\n"), data.id, *username));
  } else if (deleteCmd) {
    auto conn     = createConnection(url, token);
    auto res      = sendDELETE(conn, id);
    fmt::print("Response: {}\n", res.code);
  } else if (newGist) {
    // Create new gist from STDIN
    fmt::print("Creating gist for {}\n", *username);
    auto conn = createConnection(url, token);
    Data data = Data{id, readInput(), readInput(), readInput()};
    printData(data);

    std::string contents = createJson(data);
    auto res = createGist(conn, contents);
    printResponse(res);
  } else {
    // Create new gist from file
    fmt::print("Creating gist from file for {}\n", *username);
    auto conn = createConnection(url, token);
    for (auto& gist : app.remaining()) {
      fmt::print("{}\n", gist);
      Data data = Data{id, gist, "", readFile(gist)};
      printData(data);

      std::string contents = createJson(data);
      auto res = createGist(conn, contents);
      printResponse(res);
    }
  }

  RestClient::disable();
  return 0; 
}
