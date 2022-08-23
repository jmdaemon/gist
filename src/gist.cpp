#include "gist.h"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

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

/** Form json requests for GitHub gists */
nlohmann::json create_json(arguments args, std::string conts) {
  nlohmann::json req;
  req["description"] = args.gist.desc;
  req["public"] = !args.priv;
  req["files"][args.gist.filename]["content"] = conts;
  return req;
}

/** Create request headers */
RestClient::HeaderFields create_headers(std::string token) {
  RestClient::HeaderFields headers;
  headers["Accept"] = "application/vnd.github.v3+json";
  headers["Authorization"] = "token " + token;
  return headers;
}

/** Initiate a connection to a url */
RestClient::Connection* connect(std::string url, RestClient::HeaderFields* headers = nullptr) {
  RestClient::Connection* con = new RestClient::Connection(url);
  con->FollowRedirects(true);
  if (headers != nullptr) 
    con->SetHeaders(*headers);
  return con;
}

/** General purpose function to send http requests */
RestClient::Response send_req(RestClient::Connection* con, std::string req_type, std::string query, std::string params = "") {
  RestClient::Response response;
  switch(hash(req_type.c_str())) {
    case hash("GET")    : response = con->get(query);
    case hash("POST")   : response = con->post(query, params);
    case hash("DELETE") : response = con->del(query);
    case hash("PATCH")  : response = con->patch(query, params);
  }
  return response;
}


/** Get json object from http response */
nlohmann::json get_json(RestClient::Connection* con, std::string query) {
  auto result = nlohmann::json::parse(send_req(con, "GET", query).body);
  return result;
}

// Search
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

/** Returns searched results if the gist date is later than the specified date */
std::vector<nlohmann::json> search_date(nlohmann::json& res, std::string date, bool search_modified, RELTIME reltime) {
  std::vector<nlohmann::json> results;
  std::string date_type = (search_modified) ? "updated_at" : "created_at";
  auto tm = parse_datetime(date, GIST_DATE_FORMAT);

  // Loop through the http response
  for (auto gist : res) {
    // Get the input datetimes
    auto gist_date = gist[date_type];
    auto gist_tm = parse_datetime(gist_date, GIST_DATE_FORMAT);
    
    // Find the difference between the two dates 
    auto d1 = std::mktime(&tm);
    auto d2 = std::mktime(&gist_tm);
    auto diff = difftime(d1, d2);

    // Filter results according to reltime
    if ((diff > 0) && (reltime == AFTER)) {
      results.push_back(gist);
    } else if ((diff == 0) && (reltime == EXACT)) {
      results.push_back(gist);
    } else if ((diff < 0) && reltime == BEFORE) {
      results.push_back(gist);
    }
  };
  return results;
}

/** Return gist by matching id */
nlohmann::json search_id(nlohmann::json& res, std::string id) {
  nlohmann::json result = nullptr;
  for (auto gist : res)
      if (gist["id"] == id)
        result = gist;
  return result;
}

/** Return gists by matching filename */
std::vector<nlohmann::json> search_filename(nlohmann::json res, std::string file) {
  std::vector<nlohmann::json> results;
  for (auto gist : res["files"]) 
    if (gist["filename"] == file)
      results.push_back(gist);
  return results;
}
