#pragma once
#include <string>

namespace sketch
{
class Util
{
public:
    static long long getCurrentTimeMS();
    static std::string winkey_to_char(int key);
    static bool is_valid_length_expression(const std::string& input);
    static std::pair<float, std::string> extract_value_and_unit(const std::string& input);
    static std::string cut_string_with_two_decimals(float value);
};

}// namespace sketch
