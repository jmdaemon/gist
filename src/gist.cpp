#include "gist.h"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

/** Parse gist config file for authentication token */
std::tuple<std::string, std::string> parse_config(std::string path) {
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
nlohmann::json create_json(std::string fname, std::string conts, std::string desc, bool priv) {
  nlohmann::json req;
  req["description"] = desc;
  req["public"] = !priv;
  req["files"][fname]["content"] = conts;
  return req;
}

nlohmann::json create_json(arguments args, std::string conts) {
  nlohmann::json req;
  req["description"] = std::string(args.gist.desc, strlen(args.gist.desc));
  req["public"] = !args.priv;
  req["files"][std::string(args.gist.filename, strlen(args.gist.filename))]
    ["content"] = conts;
  return req;
}

/** Create request headers */
RestClient::HeaderFields create_headers(std::string token) {
  RestClient::HeaderFields headers;
  headers["Accept"] = GITHUB_ACCEPT;
  headers["Authorization"] = fmt::format("token {}", token);
  return headers;
}

/** Initiate a connection to a url */
RestClient::Connection* connect(std::string url, RestClient::HeaderFields* headers) {
  RestClient::Connection* con = new RestClient::Connection(url);
  con->FollowRedirects(true);
  if (headers != nullptr) {
    SPDLOG_DEBUG("Setting headers");
    con->SetHeaders(*headers);
  }
  return con;
}

/** General purpose function to send http requests */
RestClient::Response send_req(RestClient::Connection* con, std::string req_type, std::string query, std::string params) {
  switch(hash(req_type.c_str())) {
    case hash("GET")    : return con->get(query);
    case hash("POST")   : return con->post(query, params);
    case hash("DELETE") : return con->del(query);
    case hash("PATCH")  : return con->patch(query, params);
    default             : return RestClient::Response();
  }
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
      [results] (nlohmann::json& gist) mutable {
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

// Commands

/** Log HTTP response information */
void log_res(RestClient::Response res) {
  SPDLOG_DEBUG("Response Code: {}", res.code);
  SPDLOG_DEBUG("Response Headers: ");
  for (auto it : res.headers)
    SPDLOG_DEBUG("{}: {}", it.first, it.second);
  SPDLOG_DEBUG("Response Body:\n{}", res.body);
}

/* Log gist fields */
void log_gist(std::string fname, std::string conts, std::string desc, bool priv) {
  SPDLOG_DEBUG("New Gist: ");
  SPDLOG_DEBUG("Filename: {}", fname);
  SPDLOG_DEBUG("Contents:\n{}", conts);
  SPDLOG_DEBUG("Description: {}", desc);
  SPDLOG_DEBUG("Is Private: {}", priv);
}

/* Show json response */
void show_res(RestClient::Response res) {
  auto json_res = nlohmann::json::parse(res.body);
  fmt::print("{}", json_res.dump(INDENT_LEVEL));
}

/** List all gists found for the user */
void list_gists(RestClient::HeaderFields headers) {
  RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
  std::string params = "";
  std::string query = "/gists";
  auto res = send_req(con, "GET", query);
  log_res(res);
  show_res(res);
}

/** Creates a new gist from STDIN prompts */
void new_gist(arguments args, RestClient::HeaderFields headers) {
  // Read user input from STDIN
  auto fname = prompt("Filename: ");
  auto conts = prompt("Contents:\n");
  auto desc  = prompt("Description: ");
  auto priv  = (prompt("Make Private? [y/n]: ") == "y") ? true : false;

  log_gist(fname, conts, desc, priv);

  // TODO: Create automated test to check if the fields
  // match up with the inputs for the output json

  // Send http request
  RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
  auto params = create_json(args, conts);
  SPDLOG_DEBUG("JSON:\n{}", params.dump(INDENT_LEVEL));

  auto query = "/gists";
  auto res = send_req(con, "POST", query, params.dump());
  log_res(res);
  show_res(res);
}
