#pragma once
#ifndef DATA_H
#define DATA_H

#include <iosfwd>
#include <fstream>
#include <string>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

struct Data {
  std::string id, fname, desc, contents;
  bool pub = false;
};

void printData(Data data);
std::string getRaw(nlohmann::json o, std::string filter);
std::string getId(nlohmann::json o, std::string filter);
std::string getFilename(nlohmann::json o, std::string id);
std::string readInput();
void serialize(nlohmann::json data, std::string filename);

static inline std::string &ltrim(std::string &s);
static inline std::string &rtrim(std::string &s);
static inline std::string &trim(std::string &s);

#endif
