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

  RestClient::HeaderFields headers;
  headers["Accept"] = "application/vnd.github.v3+json";
  headers["Authorization"] = *token;
  conn->SetHeaders(headers);

  RestClient::Response r = conn->get("/gists");
  printResponse(r);
  
  RestClient::disable();
  return 0; 
}
