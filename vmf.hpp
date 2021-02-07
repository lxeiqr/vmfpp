#pragma once
#include <string_view>
#include <string>
#include <vector>
#include <map>

class VMF {
public:
    std::string name;
    std::vector<VMF> children;
    std::map<std::string, std::string> properties;

    int parse(std::string_view input);
    std::string serialize();
};
