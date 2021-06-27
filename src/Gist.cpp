#include <string>
#include <cstdlib> /* getenv */

// Libraries
#include <toml++/toml.h>
#include <fmt/core.h>
#include "httplib.h"

//void asioPrint() { 
  // asio
  //asio::io_context io;
  //asio::steady_timer t(io, asio::chrono::seconds(5));
  //t.wait();
  //fmt::print("Hello, world!\n");
//}

int main(int argc, char** argv) { 

  const char* GIST_FILE_PATH = std::getenv("GIST_CONFIG_HOME");
  if (GIST_FILE_PATH) {
    std::string GIST_CONFIG_HOME(GIST_FILE_PATH);
    fmt::print("$GIST_CONFIG_HOME: {}\n", GIST_CONFIG_HOME);
  }

  auto config = toml::parse_file( std::string(std::getenv("HOME")) + "/.config/gist/config.toml" );
  std::optional<std::string> token = config["user"]["token"].value<std::string>();

  // http-lib
  httplib::Client cli("https://api.github.com");
  httplib::Headers headers = { 
    { "Accept", "application/vnd.github.v3+json"},
    { "Authorization", *token}
  };
  auto res = cli.Get("/gists", headers);
  fmt::print("Response Status : {}\n", res->status);
  fmt::print("Response Body   : {}\n", res->body);
  return 0; 
}
