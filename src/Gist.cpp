#include <iostream>
#include <iomanip>
#include <fstream>
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
  return "";
}

std::string getFilename(json o, std::string id) { 
  int i = 0;
  for (auto& [gistNum, gistJSON] : o.items()) {
      //std::cout << "key: " << gistNum << ", value:" << gistJSON << '\n';
    for (auto& [key, val] : gistJSON.items()) {
        //std::cout << "Key: " << key << ", Value: " << val << std::endl;
      if (val == id) {
          std::cout << "Gist ID: " << val << std::endl;
          std::cout << gistJSON[""] << std::endl; 
          //json gistFiles = json(val["files"]);
          json currentGist = o[i];
          std::cout << currentGist.dump() << "\n\n"<< std::endl;
          json gistFiles = currentGist["files"];
          std::cout << gistFiles.dump() << std::endl; 
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
    auto conn = createConnection(url, token);
    std::string filename  = readInput();
    std::string desc      = readInput();
    std::string contents  = readInput();
    fmt::print("Gistname    : {}\n", filename);
    fmt::print("Description : {}\n", desc);
    fmt::print("Contents    : {}\n", contents);

    contents = createJson(desc, filename, contents);
    auto r = createGist(conn, contents);
    auto o = json::parse(r.body);

    //printResponse(createGist(conn, contents));
    //if (createGist(conn, contents).code == 201) {
      //fmt::print("Created a user gist\n");
    //}
  } else if (input.argExists("-u")) { 
    // Update existing gist for user
    
    if (!id.empty()) {
      /*
         If user passes in an id for the gist
            Read gistID, gistDesc, gistCont
              Make request with gistID to get filename from github
            Create json string from gistName, gistID, gistDesc, gistCont
            Send update request to /gists/{gistID} with json contents
         */

      // Get Gist remote filename
      auto conn = createConnection(url + "/users/" + *username, token);
      auto o = json::parse(getGists(conn).body); 

      //std::ofstream output("gists.json"); 
      //output << std::setw(4) << o << std::endl;
      //fname = getFilename(o, id);

      // Prepare gist json data
      fmt::print("id arg: {}\n", id);
      fname = getFilename(o, id);
      std::string desc      = readInput();
      std::string contents  = readInput();

      fmt::print("Gist ID       : {}\n", id);
      fmt::print("Gist Filename : {}\n", fname);
      fmt::print("Description   : {}\n", desc);
      fmt::print("Contents      : {}\n", contents);
      contents = createJson(desc, fname, contents); 

      // Send Gist Update Request
      conn = createConnection(url, token);
      auto r = updateGist(conn, id, contents);
      printResponse(r);
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
