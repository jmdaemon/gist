#include "gist.h"

/** Parse gist config file for authentication token */
auto parse_config(std::string path) {
  auto cfg   = toml::parse_file(path);
  auto token = cfg["user"]["token"].value<std::string>();
  auto uname = cfg["user"]["name"].value<std::string>();

  if (!token.has_value()) {
    SPDLOG_ERROR("Token must not be empty");
    exit(1);
  } else if (!uname.has_value()) {
    SPDLOG_ERROR("Username must not be empty");
    exit(2);
  }

  SPDLOG_INFO("Token    : {}", *token);
  SPDLOG_INFO("Username : {}", *uname);

  return std::make_tuple(*token, *uname);
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
