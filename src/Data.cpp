#include "Data.h"

#include <iostream>

void printData(Data data) { 
  fmt::print("==== Gist Data ====\n"); 
  fmt::print("ID          : {}\n", trim(data.id));
  fmt::print("Filename    : {}\n", trim(data.fname));
  fmt::print("Description : {}\n", trim(data.desc));
  fmt::print("Contents    : {}\n", trim(data.contents));
  fmt::print("Public      : {}\n", data.pub);
  fmt::print("\n");
}

std::string getRaw(nlohmann::json o, std::string filter) {
  for (int i=0; i < o.size(); i++) { 
    for (auto& gist : o[i]["files"]) { 
      spdlog::info("{}\n", gist["filename"]);
      if (gist["filename"] == filter) {
        return gist["raw_url"];
      }
    }
  }
  return "";
}

void serialize(nlohmann::json data, std::string filename) {
  auto out = fmt::output_file(filename); 
  out.print("{}\n", data.dump(4));
}

std::string readInput(std::string prompt) {
  std::string input = "";
  fmt::print("{}: ", prompt);
  while (std::getline(std::cin, input)) {
    return input;
  }
  return input;
}
