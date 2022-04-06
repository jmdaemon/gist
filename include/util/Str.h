#pragma once
#ifndef STR_H
#define STR_H

#include <iosfwd>
#include <string>
#include <algorithm>
#include <functional> 

std::string substitute(std::string str, const std::string& from, const std::string& to);

inline std::string &ltrim(std::string &s);
inline std::string &rtrim(std::string &s);
inline std::string &trim(std::string &s);
std::string unquote(std::string s);
#endif // STR_H
