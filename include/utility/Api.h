#pragma once
#ifndef API_H
#define API_H

#include "Data.h"
#include <toml++/toml.h>
#include <nlohmann/json.hpp>
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

struct Config { std::string url, username, token; };

constexpr unsigned int hash(const char *s, int off = 0);
void printResponse(RestClient::Response res);
std::string createJson(Data data);
auto createHeaders(std::optional<std::string> token);
RestClient::Connection* connect(std::string url, std::optional<std::string> token);
//auto send(std::string cmd, Config config, Data data, std::string query="");
RestClient::Response send(std::string cmd, Config config, Data data, std::string query="");
nlohmann::json getGists(Config& config, std::string query="?per_page=100&page=1");
void updateGist(Data& data, Config& config);

#endif // API_H
