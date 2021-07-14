#include "Data.h"

#include <iostream>

void printData(Data data) { 
  fmt::print("==== Gist Data ====\n"); 
  fmt::print("ID           : {}\n", trim(data.id));
  fmt::print("Filename     : {}\n", trim(data.fname));
  fmt::print("Description  : {}\n", trim(data.desc));
  fmt::print("Contents     : {}\n", trim(data.contents));
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
    for (auto& gist : o[i]["files"]) { 
      if (gist["filename"] == filter) {
        return o[i]["id"];
      }
    }
  }
  return "";
}

std::string getFilename(nlohmann::json o, std::string id) { 
  int i = 0;
  for (auto& [i_key, gist] : o.items()) {
    for (auto& [j_key, gistID] : gist.items()) {
      if (gistID == id) {
        nlohmann::json gistFiles = o[i]["files"];
        for (auto& [fname, values] : gistFiles.items()) {
          return fname;
        }
      }
    }
    i++;
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
