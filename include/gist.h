#pragma once
#ifndef GIST_H
#define GIST_H

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
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/ostream.h>
#include <spdlog/fmt/bundled/os.h>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

static const std::string GITHUB_API_URL = "https://api.github.com";

#endif // GIST_H
