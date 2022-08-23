#pragma once
#ifndef GIST_H
#define GIST_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// Imports
#include "gist_def.h"
#include "cli.h"

// Standard Library
#include <algorithm>
#include <exception>
#include <istream>
#include <tuple>
#include <vector>
#include <optional>
#include <string>

// Third Party Libraries
#include <jolly.h>
#include <toml++/toml.h>
#include <nlohmann/json.hpp>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

// Constants
static const std::string logger_name = "gist";
static const std::string GITHUB_API_URL = "https://api.github.com";

auto parse_config(std::string path);

#endif // GIST_H
