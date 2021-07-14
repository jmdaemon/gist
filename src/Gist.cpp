#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib> /* getenv */
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>

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

void prettyPrint(RestClient::Response res) {
  //json response = json::parse(res.body);
  //std::cout << std::setw(4) << response << std::endl;
  std::string response = json::parse(res.body).dump(4);
  //std::cout << std::setw(4) << response << std::endl;
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
  return "";
}

std::string getFilename(json o, std::string id) { 
  int i = 0;
  for (auto& [gistNum, gistJSON] : o.items()) {
      //std::cout << "key: " << gistNum << ", value:" << gistJSON << '\n';
    for (auto& [key, val] : gistJSON.items()) {
        //std::cout << "Key: " << key << ", Value: " << val << std::endl;
      if (val == id) {
          //std::cout << "Gist ID: " << val << std::endl;
          //std::cout << gistJSON[""] << std::endl; 
          //json gistFiles = json(val["files"]);
          //json currentGist = o[i];
          //std::cout << currentGist.dump() << "\n\n"<< std::endl;
          //json gistFiles = currentGist["files"];
        json gistFiles = o[i]["files"];
          //std::cout << gistFiles.dump() << std::endl; 
        for (auto& [fname, values] : gistFiles.items()) {
          std::cout << "Filename: " << fname << std::endl;
          return fname;
          //std::cout << "Gist ID: " << val << std::endl;
          //std::cout << gistJSON[""] << std::endl;
        }
      }
    }
    i++;
  }
  /*
     This code is flawed because we can't access this o[i]["files"][*]["filename"
     */
  //for (int i=0; i < o.size(); i++) { 
    //std::cout << o[i]["id"] << std::endl;
    //if (o[i]["id"] == id) {
      //std::cout << o[i] << std::endl;
      //std::cout << o[i]["files"] << std::endl;
      //std::cout << o[i]["files"]["filename"] << std::endl;
      //return o[i]["files"]["filename"];
    //}
  //}
  return "";
}

void serialize(json data, std::string filename) {
    std::ofstream output(filename);
    output << std::setw(4) << data << std::endl;
}

// C Struct
//typedef union Data_u {
    //struct Data_s { 
      //std::string id, fname, desc, contents;
    //};
    //std::string Data_a[4];
//} Data;

struct Data {
  std::string id, fname, desc, contents;
};

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

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

void printData(Data data) { 
  fmt::print("==== Gist Data ====\n"); 
  fmt::print("ID           : {}\n", trim(data.id));
  fmt::print("Filename     : {}\n", trim(data.fname));
  fmt::print("Description  : {}\n", trim(data.desc));
  fmt::print("Contents     : {}\n", trim(data.contents));
  fmt::print("\n");
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
    auto conn = createConnection(url + "/users/" + *username, token);
    auto res  = getGists(conn);
    //prettyPrint(res);
  } else if (input.argExists("-c")) { 

    // Create new gist for user
    fmt::print("Creating gist for {}\n", *username);
    auto conn = createConnection(url, token);
    Data data = Data{id, readInput(), readInput(), readInput()};
    printData(data);

    contents = createJson(data.desc, data.fname, data.contents);
    auto res   = createGist(conn, contents);
    prettyPrint(res);

  } else if (input.argExists("-u")) { 
    // Update existing gist for user
    
    if (!id.empty()) {

      // Get remote filename of gist
      auto conn = createConnection(url + "/users/" + *username, token);
      auto o = json::parse(getGists(conn).body); 

      // Serialize output if needed
      //std::ofstream output("gists.json"); 
      //output << std::setw(4) << o << std::endl;

      // Prepare gist json data
      Data data = {id, getFilename(o, id), readInput(), readInput()};
      printData(data);
      contents = createJson(data.desc, data.fname, data.contents); 

      // Send Gist Update Request
      conn     = createConnection(url, token);
      auto res = updateGist(conn, id, contents);
      prettyPrint(res);
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
