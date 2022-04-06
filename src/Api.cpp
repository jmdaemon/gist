#include "Api.h"

using nlohmann::json;

constexpr unsigned int hash(const char *s, int off) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

void printResponse(RestClient::Response res) {
  std::string response = json::parse(res.body).dump(4);
  fmt::print("{}\n", response);
}

std::string createJson(Data data) {
  json o;
  o["description"]                  = data.desc;
  o["public"]                       = data.pub;
  o["files"][data.fname]["content"] = data.contents;
  return o.dump();
}

auto createHeaders(std::optional<std::string> token) {
  RestClient::HeaderFields headers;
  headers["Accept"] = "application/vnd.github.v3+json";
  headers["Authorization"] = "token " + *token;
  return headers;
}

RestClient::Connection* connect(std::string url, std::optional<std::string> token) {
//auto connect(std::string url, std::optional<std::string> token) {
  RestClient::Connection* conn = new RestClient::Connection(url);
  conn->FollowRedirects(true);
  conn->SetHeaders(createHeaders(token)); 
  return conn;
}

//auto send(std::string cmd, Config config, Data data, std::string query) {
RestClient::Response send(std::string cmd, Config config, Data data, std::string query) {
  auto conn = connect(config.url, config.token);
  switch(hash(cmd.c_str())) {
    case hash("GET")    : return conn->get(std::string("/gists") + query);
    case hash("POST")   : return conn->post("/gists", createJson(data));
    case hash("DELETE") : return conn->del("/gists/" + data.id);
    case hash("PATCH")  : return conn->patch("/gists/" + data.id, createJson(data));
    default             : return RestClient::Response();
  }
}

json getGists(Config& config, std::string query) {
  config.url = config.url + "/users/" + config.username;
  return json::parse(send("GET", config, {}, query).body);
}

void updateGist(Data& data, Config& config) {
  printResponse(send("PATCH", config, data));
}
