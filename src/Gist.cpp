#include "Data.h"
#include "InputParser.h"

#include <cstdlib> /* getenv */

// Libraries
#include <toml++/toml.h>
#include <nlohmann/json.hpp>
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

using nlohmann::json;

void showUsage() {
  fmt::print("Show Program Usage\n");
}

void printResponse(RestClient::Response res) {
  fmt::print("Response Code     : {}\n", res.code);
  fmt::print("Response Body     : {}\n", res.body);
}

void prettyPrint(RestClient::Response res) {
  std::string response = json::parse(res.body).dump(4);
  fmt::print("{}\n", response);
}

std::string createJson(std::string desc, std::string filename, std::string content, bool pub = false) {
  json o;
  o["description"]                = desc;
  o["public"]                     = pub;
  o["files"][filename]["content"] = content;
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

auto getGists(RestClient::Connection* conn) { return conn->get("/gists"); }
auto createGist(RestClient::Connection* conn, std::string contents) { return conn->post("/gists", contents); }
auto updateGist(RestClient::Connection* conn, std::string gistID, std::string contents) { return conn->patch("/gists/" + gistID, contents); }

json listUserGists(std::string url, std::string username, std::optional<std::string> token) {
  auto conn = createConnection(url + "/users/" + username, token);
  auto o = json::parse(getGists(conn).body);
  return o;
}

void updateUserGist(Data data, std::string url, std::optional<std::string> token) {
  printData(data);
  auto contents  = createJson(data.desc, data.fname, data.contents); 
  auto conn = createConnection(url, token);
  auto res  = updateGist(conn, data.id, contents);
  prettyPrint(res);
}

int main(int argc, char** argv) {
  InputParser input(argc, argv);
  if (input.argExists("-h") || input.argExists("--help")) {
    showUsage();
    return 0;
  } 

  std::string GIST_CONFIG = (std::getenv("GIST_CONFIG_HOME")) 
    ? std::string(std::getenv("GIST_CONFIG_HOME")) : std::string(std::getenv("HOME")) + "/.config/gist/config.toml"; 

  GIST_CONFIG = (input.argExists("--config")) ? input.getArg("--config") : GIST_CONFIG;

  // Parse auth token
  auto config = toml::parse_file(GIST_CONFIG);
  const std::optional<std::string> token      = config["user"]["token"].value<std::string>();
  const std::optional<std::string> username   = config["user"]["name"].value<std::string>();
  const std::string url = "https://api.github.com";

  fmt::print("\n==== Config ====\n");
  fmt::print("Token    : [{}]\n", *token);
  fmt::print("Username : [{}]\n\n", *username);

  RestClient::init();

  std::string id    = (input.argExists("id"))     ? input.getArg("id")      : "";
  std::string fname = (input.argExists("fname"))  ? input.getArg("fname")   : "";
  std::string contents = "";

  if (input.argExists("-l")) {
    // List user gists
    fmt::print("Listing all gists for {}\n", *username);
    listUserGists(url, *username, token);
  } else if (input.argExists("-c")) { 
    // Create new gist for user
    fmt::print("Creating gist for {}\n", *username);
    auto conn = createConnection(url, token);
    Data data = Data{id, readInput(), readInput(), readInput()};
    printData(data);

    contents = createJson(data.desc, data.fname, data.contents);
    auto res = createGist(conn, contents);
    prettyPrint(res);

  } else if (input.argExists("-u")) { 
    // Update existing gist for user
    auto o = listUserGists(url, *username, token);

    if (!id.empty()) { 
      fmt::print("Updating gist {}, for {}\n", id, *username);
      Data data = {id, getFilename(o, id), readInput(), readInput()};
      updateUserGist(data, url, token);

    } else if (!fname.empty()) {
      id = getId(o, fname);
      fmt::print("Updating gist {} for {}\n", id, *username);
      
      Data data = Data{id, fname, readInput(), readInput()};
      updateUserGist(data, url, token);
    }
  }

  RestClient::disable();
  return 0; 
}
