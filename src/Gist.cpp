//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <cstdlib> /* getenv */

#include <string>
#include <fmt/core.h>

//#include <cpr/cpr.h>
#include "httplib.h"

int main(int argc, char** argv) { 
  //cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/"});

  ////fmt::print("Request URL           : {}\n", r.url);
  //std::cout<<"Request URL           : " << r.url << std::endl;
  //fmt::print("Request Status Code   : {}\n", r.status_code);                  // 200 
  //fmt::print("Request Content Type  : {}\n", r.header["content-type"]);       // application/json; charset=utf-8 
  //fmt::print("Request Text          : {}\n", r.text);                         // JSON text string



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
