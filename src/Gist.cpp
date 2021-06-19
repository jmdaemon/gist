#include <iostream>

#include <string>
#include <cpr/cpr.h>
#include <fmt/core.h>

int main(int argc, char** argv) { 
  cpr::Response r = cpr::Get(
      //{"https://api.github.com/gists"},
      //{"https://api.github.com/gists"},
      //cpr::Header{{"accept", "application/vnd.github.v3+json"}}
      //cpr::Header{{"Accept", "application/vnd.github.v3+json"}}
      //cpr::Url{"http://www.httpbin.org/headers"}, 
      //cpr::Url{"https://api.github.com/gists"},
      //cpr::Url{"https://gist.github.com/jmdaemon/gists"},
      //cpr::Header{{"accept", "application/vnd.github.v3+json"}}
      //cpr::Header{{"accept", "application/json"}}
      //cpr::Url{"https://api.getgist.com/contacts"},
      //cpr::Parameters{{"id", "josephm.diza@gmail.com"}}
      //cpr::Url{"http://www.httpbin.org/get"}
      //cpr::Url{"https://api.github.com/gists"}
      //cpr::Url{"https://api.github.com/users/octocat/orgs"},
      cpr::Url{"https://api.github.com/"},
      cpr::Header{
      {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0"}, 
      {"accept", "application/json"}},
      //{"Accept", "application/vnd.github.v3+json"}},
      cpr::Timeout{1000}
      );

  //fmt::print("Request URL           : {}\n", r.url);
  std::cout<<"Request URL           : " << r.url << std::endl;
  fmt::print("Request Status Code   : {}\n", r.status_code);                  // 200 
  fmt::print("Request Content Type  : {}\n", r.header["content-type"]);       // application/json; charset=utf-8 
  fmt::print("Request Text          : {}\n", r.text);                         // JSON text string

  //fmt::print("Request Status Code: {}\n", r.status_code);
  //fmt::print("Request Header: {}\n", r.raw_header);
  //fmt::print("Request Text: {}\n", r.text);
  return 0;
}
