#include "gist.h"

// Parse gist IDs & gist urls
auto parseID(std::string str, char delim = '/') {
  std::istringstream ss(str);
  std::string token;
  std::vector<std::string> tokens; 
  while(std::getline(ss, token, delim))
    tokens.push_back(token); 
  auto result = std::string(tokens.back());
  return result;
}

// Parse auth token
auto parseConfig(std::string GIST_CONFIG) {
  auto config   = toml::parse_file(GIST_CONFIG);
  auto token    = config["user"]["token"].value<std::string>();
  auto username = config["user"]["name"].value<std::string>();

  // Enable on verbose mode
  //fmt::print("\n==== Config ====\n");
  //fmt::print("Token    : [{}]\n", *token);
  //fmt::print("Username : [{}]\n\n", *username);

  if (token->empty()) {
    fmt::print("Token must not be empty");
    throw std::exception();
  } else if (username->empty()) {
    fmt::print("Username must not be empty");
    throw std::exception();
  }
  return std::make_tuple(*username, *token);
}

std::string substituteHyphenColon(std::string str) {
  return substitute(str, "-", ":");
}

//void search(nlohmann::json& o, Options options) {
    //if (!options.raw) {
      //fmt::print("{}\n", o.dump(4));
      //return;
    //}
    //std::for_each(o["files"].begin(), o["files"].end(), [] (json& gist) {
        //std::string s = gist["raw_url"];
        //fmt::print("{}\n", unquote(s));
        //});
//}

//int searchDate(nlohmann::json o, arguments args, std::string date, std::string gistDate) {
  //std::for_each(o.begin(), o.end(), std::bind([] (nlohmann::json o, Options options, std::string date, std::string gistDate) {
    //std::string gist_date  = substituteHyphenColon(gistDate);
    //if (gist_date > date) { search(o, options); } // Retrieve gists later than date
    //} , std::placeholders::_1, options, date, gistDate));
  //return cleanup();
//}

//inline int searchID(nlohmann::json o, Options options) {
  //std::for_each(o.begin(), o.end(), std::bind([] (json o, Options options) {
    //if (o["id"] == options.searchID) { search(o, options); } return;
    //} , std::placeholders::_1, options));
  //return cleanup();
//}

//inline int searchFile(nlohmann::json o, Options options) {
  //std::for_each(o.begin(), o.end(), std::bind([] (json o, Options options) {
        //for (auto& file : o["files"]) { if (file["filename"] == options.hasName) search(o, options); }
        //}, std::placeholders::_1, options));
  //return cleanup();
//}
