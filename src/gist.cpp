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

/** Returns searched results as raw urls
  TODO: Return nlohmann json object instead with the full objects instead. */
std::vector<std::string> search(nlohmann::json& res) {
  auto files = res["files"];
  std::vector<std::string> results;
  std::for_each(files.begin(), files.end(),
      [] (nlohmann::json& gist, std::vector<std::string> results) {
        std::string s = gist["raw_url"];
        results.push_back(unquote(s));
      });
  return results;
}

/** Parse arbitrary datetime strings
   e.g: 2021-07-14T02:10:41Z */
std::tm parse_datetime(std::string datetime, std::string format) {
  std::tm tm = {};
  std::stringstream ss(datetime);
  ss >> std::get_time(&tm, format.c_str());
  return tm;
}

/** Returns searched results if the gist */
//nlohmann::json search_date(nlohmann::json& res, std::string date, bool search_modified) {
  //nlohmann::json results;
  //std::string date_type = (search_modified) ? "updated_at" : "created_at";

  //std::for_each(res.begin(), res.end(), [] (nlohmann::json& gist) {
      //std::string gist_date  = substituteHyphenColon(gistDate);
      //// 2021-07-14T02:10:41Z
      //if()

      //});

  //return results;

  //std::for_each(res.begin(), res.end(), [] (nlohmann::json& gist) {


      //std::string gist_date  = substituteHyphenColon(gistDate);
  //if (gist_date > date) { search(o, options); } // Retrieve gists later than date
  //} , std::placeholders::_1, options, date, gistDate);

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
