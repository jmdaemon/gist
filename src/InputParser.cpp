#include "InputParser.h"

InputParser::InputParser (int &argc, char **argv) {
    for (int i=1; i < argc; ++i)
        this->tokens.push_back(std::string(argv[i]));
}

const std::string& InputParser::getArg(const std::string &option) const {
    std::vector<std::string>::const_iterator itr;
    itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
    if (itr != this->tokens.end() && ++itr != this->tokens.end()){
        return *itr;
    }
    static const std::string empty_string("");
    return empty_string;
}

bool InputParser::argExists(const std::string &option) const {
    return std::find(this->tokens.begin(), this->tokens.end(), option)
           != this->tokens.end();
}

bool InputParser::toggleOption(std::string shortFlag, std::string longFlag) const {
    if (this->argExists(shortFlag) || this->argExists(longFlag)) {
      return true;
    }
    return false;
}

bool InputParser::toggleOption(std::string flag) const {
    if (this->argExists(flag)) {
      return true;
    }
    return false;
}
