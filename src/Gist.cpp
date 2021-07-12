#include <iostream>
#include <string>
#include <cstdlib> /* getenv */

// Libraries
#include <fmt/core.h>
#include <toml++/toml.h>
#include <nlohmann/json.hpp>

#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

#include "InputParser.h"

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

RestClient::Connection* createConnection(std::string url, std::optional<std::string> token) {
  RestClient::Connection* conn = new RestClient::Connection(url);
  conn->FollowRedirects(true);
  conn->SetHeaders(createHeaders(token)); 
  return conn;
}

std::string getRaw(json o, std::string filter) {
  for (int i=0; i < o.size(); i++) { 
    for (auto& gist : o[i]["files"]) { 
      std::cout << gist["filename"] << std::endl;
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
      std::cout << gist["filename"] << std::endl;
      if (gist["filename"] == filter) {
        return o[i]["id"];
      }
    }
  }
  return  "";
}

std::string readInput() {
    //std::string filename;
    //std::string desc;
    //std::string contents;
    //std::cin >> filename;
    //std::cin >> desc;
    //std::cin >> contents;
  std::string input = "";
  while (std::getline(std::cin, input)) {
    //std::cout << line << std::endl;
    return input;
  }
  return input;
}

void showUsage() {
  fmt::print("Show Program Usage\n");
}

auto getGists(RestClient::Connection* conn) { return conn->get("/gists"); }
auto createGist(RestClient::Connection* conn, std::string contents) { return conn->post("/gists", contents); }
auto updateGist(RestClient::Connection* conn, std::string gistID, std::string contents) { return conn->patch("/gists/" + gistID, contents); }

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
  fmt::print("Token: [{}]\tUsername: [{}]\n", *token, *username);

  RestClient::init();

  std::string id    = (input.argExists("id"))     ? input.getArg("id")      : "";
  std::string fname = (input.argExists("fname"))  ? input.getArg("fname")   : "";
  std::string contents = "";
  if (input.argExists("-l")) {
    // List user gists
    auto conn = createConnection(url + "/users/" + *username, token);
    //printResponse(getGists(conn));
    if (getGists(conn).code == 200) {
      fmt::print("Listed user gists\n");
    }
  } else if (input.argExists("-c")) { 
    // Create new gist for user
    //auto conn = createConnection(url, token);
    std::string filename  = readInput();
    std::string desc      = readInput();
    std::string contents  = readInput();
    fmt::print("Gistname    : {}\n", filename);
    fmt::print("Description : {}\n", desc);
    fmt::print("Contents    : {}\n", contents);

    contents = createJson(desc, filename, contents);
    //printResponse(createGist(conn, contents));
    if (createGist(conn, contents).code == 201) {
      fmt::print("Created a user gist\n");
    }
  } else if (input.argExists("-u")) { 
    // Update existing gist for user
    if (!id.empty()) {
      auto conn = createConnection(url + "/users/" + *username, token);
      std::string gist      = readInput();
      std::string desc      = readInput();
      std::string contents  = readInput();
      contents = createJson(desc, gist, contents);
      conn = createConnection(url, token);
      printResponse(updateGist(conn, id, contents));
    } else if (!fname.empty()) {
      auto conn = createConnection(url + "/users/" + *username, token);
      RestClient::Response r = getGists(conn);
      auto o = json::parse(r.body);
      std::string gist = readInput();
      std::string gistID = getId(o, gist);
      std::string gistURL = getRaw(o, gist);
      //fmt::print("Test  URL : {}\n", gistURL);
      //fmt::print("Test  ID  : {}\n", gistID); 
      std::string desc      = readInput();
      std::string contents  = readInput();
      contents = createJson(desc, gist, contents);
      conn = createConnection(url, token);
      printResponse(updateGist(conn, gistID, contents));
    }
  }

  RestClient::disable();
  return 0; 
}
