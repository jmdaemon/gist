#include "gist.h"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

// Helper functions

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
  SPDLOG_DEBUG("Private: {}", priv);
}

/** Finalizes a request with the given query, params strings */
void http_send(RestClient::HeaderFields headers, std::string req_type, std::string query, std::string params) {
  RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
  auto res = send_req(con, req_type, query, params);
  log_res(res);
  show_res(res);
}

/* Show json response */
void show_res(RestClient::Response res) {
  if (!res.body.empty()) {
    auto json_res = nlohmann::json::parse(res.body);
    fmt::print("{}", json_res.dump(INDENT_LEVEL));
  }
}

/** Show filtered gist results */
void show_results(std::vector<nlohmann::json> gists, bool urls_only) {
  auto show_urls = [](nlohmann::json &gist) { if (gist.contains("raw_url")) fmt::print("{}\n", gist["raw_url"]); };
  auto show_json = [](nlohmann::json &gist) { if (!gist.empty()) fmt::print("{}\n", gist.dump(INDENT_LEVEL)); };

  SPDLOG_DEBUG("Results:");
  (urls_only && (gists != nullptr) && (gists.size() > 0) && !gists.empty())
    ? std::for_each(gists.begin(), gists.end(), show_urls)
    : std::for_each(gists.begin(), gists.end(), show_json);
}

// JSON

/** Form json requests for GitHub gists */
nlohmann::json create_json(std::string fname, std::string conts, std::string desc, bool priv) {
  nlohmann::json req;
  req["description"] = desc;
  req["public"] = !priv;
  req["files"][fname]["content"] = conts;
  return req;
}

nlohmann::json create_json(arguments args) {
  return create_json(args.gist.filename, args.gist.conts, args.gist.desc, args.priv);
}

/** Returns searched results if the gist date is later than the specified date */
nlohmann::json search_date(nlohmann::json& gist, std::string date, std::string date_type, RELTIME reltime) {
  nlohmann::json result;
  auto gist_date = gist[date_type];

  // Filter results according to reltime
  if ((date == gist_date) && (reltime == EXACT))
    result = gist;
  else if ((date < gist_date) && (reltime == AFTER))
    result = gist;
  else if ((date > gist_date) && (reltime == BEFORE))
    result = gist;
  return result;
}

std::vector<nlohmann::json> filter_null(std::vector<nlohmann::json>& results) {
  results.erase(std::remove_if(results.begin(), results.end(), [] (nlohmann::json& gist) {
        return ((gist == nullptr) || (gist == NULL) || gist.empty()) ? true : false;
        }), results.end());
  return results;
}

/** Performs filtering by id, file name or date */
std::vector<nlohmann::json> filter_gists(arguments args, nlohmann::json json_res) {
  const char* filter_type = args.args[1];
  std::vector<nlohmann::json> results;

  switch(hash(filter_type)) {
    case hash("id"): { // Filter by id
      auto id = std::string(args.gist.id);
      SPDLOG_DEBUG("Gist ID: {}", id);

      for (auto gist: json_res) {
        if (gist["id"] == id) {
          results.push_back(gist);
          break;
        }
      }
      break;
    }
    case hash("name"): { // Filter by file name
      auto fname = std::string(args.gist.filename);
      SPDLOG_DEBUG("Filename: {}", fname);

      for (auto gist: json_res)
        for (auto file: gist["files"])
          if (file["filename"] == fname) 
            results.push_back(gist);
      break;
    }
    case hash("date"): {
      auto reltime = args.reltime;
      auto date = args.gist.creation;

      SPDLOG_DEBUG("Creation Date: {}", date);
      auto date_type = (args.search_modified) ? "updated_at" : "created_at";

      for (auto gist: json_res) 
        results.push_back(search_date(gist, date, date_type, reltime));

      break;
     }
  }
  results = filter_null(results); // Remove all null elements
  return results;
}
// HTTP requests

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

// Gist commands

/** List all gists found for the user */
void list_gists(RestClient::HeaderFields headers) {
  http_send(headers, "GET", GIST_ENDPOINT);
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

  auto params = create_json(fname, conts, desc, priv);
  SPDLOG_DEBUG("JSON:\n{}", params.dump(INDENT_LEVEL));
  http_send(headers, "POST", GIST_ENDPOINT, params.dump());
}

/** Creates a new gist from cli options */
void create_gist(arguments args, RestClient::HeaderFields headers) {
  auto fname  = std::string(args.gist.filename);
  auto conts  = std::string(args.gist.conts);
  auto desc   = std::string(args.gist.desc);
  auto priv   = args.priv;
  log_gist(fname, conts, desc, priv);

  // Send http request
  auto params = create_json(fname, conts, desc, priv);
  SPDLOG_DEBUG("JSON:\n{}", params.dump(INDENT_LEVEL));
  http_send(headers, "POST", GIST_ENDPOINT, params.dump());
}

/** Deletes a gist from matching id */
void delete_gist(arguments args, RestClient::HeaderFields headers) {
  auto filter_type = args.args[1];
  if ((filter_type == nullptr) || (strcmp(filter_type, "") == 0)) {
    auto id = std::string(args.gist.id);
    SPDLOG_DEBUG("Gist ID: {}", id);

    auto query = fmt::format("{}/{}", GIST_ENDPOINT, id);
    http_send(headers, "DELETE", query);
  } else {
    // Get and filter
    RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
    auto res = send_req(con, "GET", GIST_ENDPOINT);
    auto json_res = nlohmann::json::parse(res.body);
    auto results = filter_gists(args, json_res);

    // Wrap up the ids
    std::vector<std::string> ids;
    for (auto gist: results)
      ids.push_back(gist["id"]);

    // Send the requests
    for (auto id: ids) {
      fmt::print("Deleting: {}", id);
      auto query = fmt::format("{}/{}", GIST_ENDPOINT, id);
      http_send(headers, "DELETE", query);
    }
  }
}

/** Updates a gist from matching id */
void update_gist(arguments args, RestClient::HeaderFields headers) {
  auto id     = std::string(args.gist.id);
  auto fname  = std::string(args.gist.filename);
  auto conts  = std::string(args.gist.conts);
  auto desc   = std::string(args.gist.desc);
  auto nname  = std::string(args.gist.rename);
  auto priv   = args.priv;

  SPDLOG_DEBUG("Gist ID: {}", id);
  log_gist(fname, conts, desc, priv);

  auto query = fmt::format("{}/{}", GIST_ENDPOINT, id);
  nlohmann::json params;
  params["public"] = !priv;

  // Intelligently update only fields that are specified (non-null)
  if (!desc.empty())
    params["description"] = desc;

  if (!conts.empty())
    params["files"][fname]["content"] = conts;

  if (!nname.empty()) 
    params["files"][fname]["filename"] = nname; // Rename gist file

  SPDLOG_DEBUG("JSON:\n{}", params.dump(INDENT_LEVEL));
  http_send(headers, "PATCH", query, params.dump());
}

/** Searches gists by ID, filename, or date */
void search_gist(arguments args, RestClient::HeaderFields headers) {
  // TODO: When stubbing the implementation for these functions, use prepared json files
  RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
  auto res = send_req(con, "GET", GIST_ENDPOINT);
  auto json_res = nlohmann::json::parse(res.body);
  auto results = filter_gists(args, json_res);
  show_results(results, !args.raw);
}
