#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "CSSRule.h"

class CSSParser {
public:
    static std::unordered_map<std::string, CSSRule> parseFile(const std::string& filename);
    static void writeToFile(const std::string& filename, const std::unordered_map<std::string, CSSRule>& rules);

private:
    static void trimString(std::string& str);
};
