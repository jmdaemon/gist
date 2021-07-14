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
  fmt::print("Usage: gist [id/filename] [desc] [contents]");
  fmt::print("  -h, --help      Show this message and exit\n");
  fmt::print("  -c,             Create a new gist\n");
  fmt::print("  -u id,          Update an existing gist specified by id\n");
  fmt::print("  -u fname,       Update an existing gist specified by filename\n\n");
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

  if (input.argExists("-l")) {
    // List user gists

    fmt::print("Listing all gists for {}\n", *username);
    listGists(url, *username, token);
  } else if (input.argExists("-c")) { 

    // Create new gist for user
    fmt::print("Creating gist for {}\n", *username);
    auto conn = createConnection(url, token);
    Data data = Data{id, readInput(), readInput(), readInput()};
    printData(data);

    std::string contents = createJson(data);
    auto res = createGist(conn, contents);
    printResponse(res);

  } else if (input.argExists("-u")) { 
    // Update existing gist for user
    auto o = listGists(url, *username, token);
    Data data;
    if (!id.empty())
      data = {id, getFilename(o, id), readInput(), readInput()};
    else if (!fname.empty())
      data = Data{getId(o, fname), fname, readInput(), readInput()};

    updateGist(data, url, token, fmt::format(fmt::runtime("Updating gist {} for {}\n"), data.id, *username));
  }
  RestClient::disable();
  return 0; 
}
