#include "Data.h"

#include <iostream>
//#include <ranges>

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
      std::cout << gist["filename"] << std::endl;
      if (gist["filename"] == filter) {
        return gist["raw_url"];
      }
    }
  }
  return "";
}

std::string getId(nlohmann::json o, std::string filter) {
  for (int i=0; i < o.size(); i++) {
    std::string fname = (*o[i]["files"].begin())["filename"];
    fmt::print("fname: {}", fname);
    if (fname == filter) {
      fmt::print("o[i][id]: {}", o[i]["id"]);
      return o[i]["id"];
    }
  }
  return "";
}

std::string getFilename(nlohmann::json o, std::string id) { 
  for (int i = 0; i < o.size(); i++) {
    if (nlohmann::json(o[i])["id"] == id) {
      std::string result( (*o[i]["files"].begin())["filename"]);
      return result;
    }
  }
  return "";
}

void serialize(nlohmann::json data, std::string filename) {
  auto out = fmt::output_file(filename); 
  out.print("{}\n", data.dump(4));
}

std::string readInput() {
  std::string input = "";
  while (std::getline(std::cin, input)) {
    return input;
  }
  return input;
}

// trim from start
inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}
