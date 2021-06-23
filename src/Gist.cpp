//#include <iostream>
#include <string>
#include <cstdlib> /* getenv */

#include <asio.hpp>
#include <fmt/core.h>

#include "httplib.h"

int main(int argc, char** argv) { 

  const char* GIST_FILE_PATH = std::getenv("GIST_CONFIG_HOME");
  if (GIST_FILE_PATH) {
    //std::string GIST_CONFIG_HOME(std::getenv("GIST_CONFIG_HOME"));
    std::string GIST_CONFIG_HOME(GIST_FILE_PATH);
    fmt::print("$GIST_CONFIG_HOME: {}\n", GIST_CONFIG_HOME);
  }

  // http-lib
  httplib::Client cli("https://api.github.com");
  httplib::Headers headers = { 
    { "Accept", "application/vnd.github.v3+json"}
  };
  auto res = cli.Get("/gists", headers);
  fmt::print("Response Status : {}\n", res->status);
  fmt::print("Response Body   : {}\n", res->body);
  return 0;
}
