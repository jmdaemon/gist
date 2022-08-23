#pragma once
#ifndef DATA_H
#define DATA_H

#include <jolly.h>
#include <cctype>
#include <locale>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/fmt/bundled/ostream.h"
#include "spdlog/fmt/bundled/os.h"

#include <nlohmann/json.hpp>

extern "C" {
#include "file.h"
  char* read_file(const char* path);
}

struct Data {
  std::string id, fname, desc, contents;
  bool pub = false; 
};

void printData(Data data);
void serialize(nlohmann::json data, std::string filename);

std::string getRaw(nlohmann::json o, std::string filter);
std::string readInput(std::string prompt = "");

#endif
