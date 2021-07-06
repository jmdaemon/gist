#include <iostream>
#include <string>
#include <cstdlib> /* getenv */

// Libraries
#include <fmt/core.h>
#include <toml++/toml.h>
#include <nlohmann/json.hpp>

#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

using nlohmann::json;

//void asioPrint() { 
  // asio
  //asio::io_context io;
  //asio::steady_timer t(io, asio::chrono::seconds(5));
  //t.wait();
  //fmt::print("Hello, world!\n");
//}

void printResponse(RestClient::Response res) {
  fmt::print("Response Code     : {}\n", res.code);
  fmt::print("Response Body     : {}\n", res.body);
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

std::string getRaw(json o, std::string filter) {
  for (int i=0; i < o.size(); i++) { 
    for (auto& gist : o[i]["files"]) {
      if (gist["filename"] == filter) {
        return gist["raw_url"];
      }
    }
  }
  return "";
}

std::string getId(json o, std::string filter) {
  for (int i=0; i < o.size(); i++) { 
    for (auto& gist : o[i]["files"]) {
      if (gist["filename"] == filter) {
        return o[i]["id"];
      }
    }
  }
  return  "";
}

auto getGists(RestClient::Connection* conn) { return conn->get("/gists"); }
auto createGist(RestClient::Connection* conn, std::string contents) { return conn->post("/gists", contents); }
auto updateGist(RestClient::Connection* conn, std::string gistID, std::string contents) { return conn->patch("/gists/" + gistID, contents); }

int main(int argc, char** argv) { 

  // Check if GIST_CONFIG_HOME env var is defined
  const char* GIST_FILE_PATH = std::getenv("GIST_CONFIG_HOME");
  if (GIST_FILE_PATH) {
    std::string GIST_CONFIG_HOME(GIST_FILE_PATH);
    fmt::print("$GIST_CONFIG_HOME: {}\n", GIST_CONFIG_HOME);
  }

  // Parse auth token
  auto config = toml::parse_file( std::string(std::getenv("HOME")) + "/.config/gist/config.toml" );
  std::optional<std::string> token = config["user"]["token"].value<std::string>();

  const std::string url = "https://api.github.com";

  RestClient::init();
  RestClient::Connection* conn = new RestClient::Connection(url);
  conn->FollowRedirects(true);
  conn->SetHeaders(createHeaders(token));

  std::string contents = createJson("Example json file for use in GitHub REST API", "TestFile.txt", "This is a test file.");
  RestClient::Response r = getGists(conn);
  //printResponse(r);
  
  auto o = json::parse(r.body);

  fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
  fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
  fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
  fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

  std::string gistID = getId(o, "TestFile.txt");
  std::string gistURL = getRaw(o, "TestFile.txt");
  fmt::print("Test  URL : {}\n", gistURL);
  fmt::print("Test  ID  : {}\n", gistID); 
  contents = createJson("Updated json file for GitHub Gists", "TestFile.txt", "This is an updated test file.");
  printResponse(updateGist(conn, gistID, contents));
  
  RestClient::disable();
  return 0; 
}
