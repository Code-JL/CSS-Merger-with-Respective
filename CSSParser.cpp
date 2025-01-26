#include "CSSParser.h"
#include <fstream>
#include <regex>

std::unordered_map<std::string, CSSRule> CSSParser::parseFile(const std::string& filename) {
    std::unordered_map<std::string, CSSRule> rules;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::string line;
    std::string currentSelector;

    while (std::getline(file, line)) {
        trimString(line);

        if (line.empty()) continue;

        // Basic selector detection
        if (line.find('{') != std::string::npos) {
            currentSelector = line.substr(0, line.find('{'));
            trimString(currentSelector);
            rules.emplace(currentSelector, CSSRule(currentSelector));
        }
        // Basic property detection
        else if (line.find(':') != std::string::npos && line.find('}') == std::string::npos) {
            size_t colonPos = line.find(':');
            std::string propName = line.substr(0, colonPos);
            std::string propValue = line.substr(colonPos + 1);

            // Remove semicolon if present
            if (propValue.back() == ';') {
                propValue.pop_back();
            }

            trimString(propName);
            trimString(propValue);

            rules[currentSelector].properties.push_back({ propName, propValue });
        }
    }

    return rules;
}

void CSSParser::writeToFile(const std::string& filename, const std::unordered_map<std::string, CSSRule>& rules) {
    std::ofstream outFile(filename);

    for (const auto& rule : rules) {
        outFile << rule.second.selector << " {\n";
        for (const auto& prop : rule.second.properties) {
            outFile << "    " << prop.name << ": " << prop.value << ";\n";
        }
        outFile << "}\n\n";
    }
}

void CSSParser::trimString(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}
