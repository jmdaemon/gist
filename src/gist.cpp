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
//std::vector<std::string> search(nlohmann::json& res) {
std::vector<nlohmann::json> search(nlohmann::json& res) {
  auto files = res["files"];
  //std::vector<std::string> results;
  std::vector<nlohmann::json> results;

  for (auto file: files) {
      //SPDLOG_DEBUG("{}", file.dump(INDENT_LEVEL));
      //std::string s = file["raw_url"];
      //SPDLOG_DEBUG("{}", s);
      //results.push_back(s);
      results.push_back(file);
  }
  //std::for_each(files.begin(), files.end(),
      //[results] (nlohmann::json& gist) mutable {
        ////results.push_back(s);
      //});


  //std::for_each(files.begin(), files.end(),
      //[results] (nlohmann::json& gist) mutable {
        //SPDLOG_DEBUG("{}", gist.dump(INDENT_LEVEL));
        //std::string s = gist["raw_url"];
        //SPDLOG_DEBUG("{}", s);
        ////results.push_back(unquote(s));
        //results.push_back(s);
        ////results.push_back(s);
      //});
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
  SPDLOG_DEBUG("Parsing Datetime: {}", date);
  auto tm = parse_datetime(date, GIST_DATE_FORMAT);
  SPDLOG_DEBUG("Datetime: {}", date);

  auto gist = res;
  // Loop through the http response
  //for (auto gist : res) {
    // Get the input datetimes
    std::string gist_date = gist[date_type];
    SPDLOG_DEBUG("Gist Date: {}", gist_date);
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
  //};
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

/** Deletes a gist from matching id
TODO: Add option to mass delete gists by matching filename, or by dates */
void delete_gist(arguments args, RestClient::HeaderFields headers) {
  auto id = std::string(args.gist.id);
  SPDLOG_DEBUG("Gist ID: {}", id);

  auto query = fmt::format("{}/{}", GIST_ENDPOINT, id);
  http_send(headers, "DELETE", query);
}

/** Updates a gist from matching id
TODO: Add option to mass update gists by matching filename, or by dates */
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

/** Show results */
void show_results(std::vector<std::vector<nlohmann::json>> results, bool urls_only) {
  SPDLOG_DEBUG("Results:");
  if (urls_only) {
    for (auto it: results)
      for (auto gist : it)
        fmt::print("{}\n", gist["raw_url"]);
  } else {
    for (auto it: results)
      for (auto gist : it)
        fmt::print("{}\n", gist.dump(INDENT_LEVEL));
  }
}

/** Searches gist by ID, filename, or date
TODO: Add option to mass update gists by matching filename, or by dates */
void search_gist(arguments args, RestClient::HeaderFields headers) {
  // TODO: When stubbing the implementation for these functions, use prepared json files
  RestClient::Connection* con = connect(GITHUB_API_URL, &headers);
  auto res = send_req(con, "GET", GIST_ENDPOINT);
  auto json_res = nlohmann::json::parse(res.body);
  auto results = filter_gists(args, json_res);
  show_results(results, false);
}

/** Performs filtering by id, file name or date */
std::vector<std::vector<nlohmann::json>> filter_gists(arguments args, nlohmann::json json_res) {
  auto filter_type = args.args[1];
  std::vector<std::vector<nlohmann::json>> results;

  // Gist ID filtering
  if (strcmp(filter_type, "id") == 0) {
    auto id = std::string(args.gist.id);
    SPDLOG_DEBUG("Gist ID: {}", id);

    for (auto gist: json_res) {
      if (gist["id"] == id) {
        std::vector<nlohmann::json> container;
        container.push_back(gist);
        results.push_back(container);
        break;
      }
    }
  } else if (strcmp(filter_type, "name") == 0) {
    auto fname = std::string(args.gist.filename);
    SPDLOG_DEBUG("Filename: {}", fname);

    for (auto gist: json_res)
      for (auto file: gist)
        if (file["filename"] == fname) 
          results.push_back(gist);

  } else if (strcmp(filter_type, "date") == 0) {
    auto reltime = args.reltime;
    auto date = args.gist.creation;
    // Do creation date for now
    SPDLOG_DEBUG("Creation Date: {}", date);

    for (auto gist: json_res) {
      //SPDLOG_DEBUG("Gist: {}", gist.dump(INDENT_LEVEL));
      results.push_back(search_date(gist, date, false, reltime));
    }

    //for (auto gist: json_res) {
    //}
    //switch(reltime) {
      //case AFTER: break;
      //case BEFORE: break;
      //case EXACT: break;
    //}
  } else {
  }
  return results;
}

