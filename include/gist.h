#pragma once
#ifndef GIST_H
#define GIST_H

// Enable SPDLOG macros
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// Imports
#include "gist_def.h"
#include "cli.h"

// Standard Library
#include <algorithm>
#include <cstdio>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

// Third Party Libraries
#include <jolly.h>
#include <jolly_spdlog.h>
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

USE_JOLLY_HASH_FUNCTION

// Constants
static const std::string logger_name = "gist";
static const std::string logger_format = "[%Y-%m-%d %H.%M.%S.%e] [%^%l%$] [%s:%#] %v"; /* [2022-08-20 23:16:43.347] [debug] [app.cpp:3] Message! */
static const std::string GITHUB_API_URL = "https://api.github.com";
static const std::string GITHUB_ACCEPT = "application/vnd.github+json";
static const std::string GIST_ENDPOINT = "/gists";
static const std::string GIST_DATE_FORMAT = "%FT%T%z"; // ISO 8601 Format, 2021-07-14T02:10:41Z
static const std::string GIST_CONFIG_PATH = std::string(getenv("HOME")) + "/.config/gist/config.toml";
static const unsigned int INDENT_LEVEL = 4;

// Helper functions
std::tuple<std::string, std::string> parse_config(std::string path);

void log_res(RestClient::Response res);
void log_gist(RestClient::Response res);
void http_send(RestClient::HeaderFields headers, std::string req_type, std::string query, std::string params = "");
void show_res(RestClient::Response res);
void show_results(std::vector<nlohmann::json> gists, bool urls_only);

// JSON
nlohmann::json create_json(arguments args);
nlohmann::json create_json(std::string fname, std::string conts, std::string desc, bool priv);
nlohmann::json search_date(nlohmann::json& gist, std::string date, std::string date_type, RELTIME reltime);
std::vector<nlohmann::json> filter_null(std::vector<nlohmann::json>& results);
std::vector<nlohmann::json> filter_gists(arguments args, nlohmann::json json_res);

// HTTP requests
RestClient::HeaderFields create_headers(std::string token);
RestClient::Connection* connect(std::string url, RestClient::HeaderFields* headers = nullptr);
RestClient::Response send_req(RestClient::Connection* con, std::string req_type, std::string query, std::string params = "");

// Gist commands
void list_gists(RestClient::HeaderFields headers);
void show_gist(arguments args, RestClient::HeaderFields headers);
void new_gist(arguments args, RestClient::HeaderFields headers);
void create_gist(arguments args, RestClient::HeaderFields headers);
void delete_gist(arguments args, RestClient::HeaderFields headers);
void update_gist(arguments args, RestClient::HeaderFields headers);
void search_gist(arguments args, RestClient::HeaderFields headers);

#endif // GIST_H
