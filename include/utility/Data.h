#pragma once
#ifndef DATA_H
#define DATA_H

//#include <iosfwd>
//#include <string>
//#include <algorithm>
//#include <functional> 
#include "Str.h"
#include <cctype>
#include <locale>

//#include <fmt/core.h>
//#include <fmt/os.h>
//#include <fmt/ostream.h>
#include "spdlog/spdlog.h"
//#include <fmt/ostream.h>
//#include <fmt/os.h>
//#include "spdlog/fmt/fmt.h"
//#include "spdlog/fmt/bundled/format.h"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/fmt/bundled/ostream.h"
#include "spdlog/fmt/bundled/os.h"

//#define SPDLOG_FMT_EXTERNAL
#include <nlohmann/json.hpp>

struct Data {
  std::string id, fname, desc, contents;
  bool pub = false; 
};

void printData(Data data);
void serialize(nlohmann::json data, std::string filename);
std::string getRaw(nlohmann::json o, std::string filter);
std::string readInput(std::string prompt = "");

//inline std::string &ltrim(std::string &s);
//inline std::string &rtrim(std::string &s);
//inline std::string &trim(std::string &s);

#endif
