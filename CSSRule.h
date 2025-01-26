#pragma once
#include <string>
#include <vector>

struct CSSProperty {
    std::string name;
    std::string value;
};

class CSSRule {
public:
    std::string selector;
    std::vector<CSSProperty> properties;

    CSSRule() = default;

    CSSRule(const std::string& sel) : selector(sel) {}
};
