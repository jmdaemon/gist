#pragma once
#ifndef GIST_H
#define GIST_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// Imports
#include "gist_def.h"
#include "cli.h"

// Standard Library
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <exception>
#include <iomanip>
#include <istream>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

// Third Party Libraries
#include <jolly.h>
#include <toml++/toml.h>
#include <nlohmann/json.hpp>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

// Constants
static const std::string logger_name = "gist";
/* [2022-08-20 23:16:43.347] [debug] [app.cpp:3] Message! */
static const std::string logger_format = "[%Y-%m-%d %H.%M.%S.%e] [%^%l%$] [%s:%#] %v";

static const std::string GITHUB_API_URL = "https://api.github.com";
static const std::string GITHUB_ACCEPT = "application/vnd.github+json";
static const std::string GIST_ENDPOINT = "/gists";
static const std::string GIST_DATE_FORMAT = "%FT%T%z"; // ISO 8601 Format, 2021-07-14T02:10:41Z
static const std::string GIST_CONFIG_PATH = std::string(getenv("HOME")) + "/.config/gist/config.toml";
static const unsigned int INDENT_LEVEL = 4;

// Gist functions
std::tuple<std::string, std::string> parse_config(std::string path);
//std::vector<std::string> search(nlohmann::json& res);
std::tm parse_datetime(std::string datetime, std::string format);

// HTTP requests
RestClient::HeaderFields create_headers(std::string token);
RestClient::Connection* connect(std::string url, RestClient::HeaderFields* headers = nullptr);
RestClient::Response send_req(RestClient::Connection* con, std::string req_type, std::string query, std::string params = "");

// Json
nlohmann::json create_json(arguments args, std::string conts);
nlohmann::json get_json(RestClient::Connection* con, std::string query);

// Gist search functions
//std::vector<std::string> search(nlohmann::json& res);
//std::vector<nlohmann::json> search(nlohmann::json& res);
std::vector<nlohmann::json> search_date(nlohmann::json& res, std::string date, bool search_modified, RELTIME reltime);
nlohmann::json search_id(nlohmann::json& res, std::string id);
std::vector<nlohmann::json> search_filename(nlohmann::json res, std::string file);

// Gist commands
void log_res(RestClient::Response res);
void log_gist(RestClient::Response res);
void http_send(RestClient::HeaderFields headers, std::string req_type, std::string query, std::string params = "");
void show_res(RestClient::Response res);

//std::vector<std::vector<nlohmann::json>> filter_gists(arguments args, RestClient::HeaderFields headers, nlohmann::json json_res);
std::vector<std::vector<nlohmann::json>> filter_gists(arguments args, nlohmann::json json_res);

void list_gists(RestClient::HeaderFields headers);
void new_gist(arguments args, RestClient::HeaderFields headers);
void create_gist(arguments args, RestClient::HeaderFields headers);
void delete_gist(arguments args, RestClient::HeaderFields headers);
void update_gist(arguments args, RestClient::HeaderFields headers);
void search_gist(arguments args, RestClient::HeaderFields headers);

#endif // GIST_H
