#include "sketch/Util/Util.h"
#include <chrono>
#include <regex>

namespace sketch
{
long long Util::getCurrentTimeMS()
{
    auto begin = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(begin.time_since_epoch()).count();
}

std::string Util::winkey_to_char(int key)
{
    if(key >= 96 && key <= 105)
    {// 数字键
        return std::string(1, '0' + (key - 96));
    }
    else if(key >= 48 && key <= 57)
    {// 数字键
        return std::string(1, '0' + (key - 48));
    }
    else if(key == 109)
    {// -
        return "-";
    }
    else if(key == 110 || key == 190)
    {// 小数点键
        return ".";
    }
    return "";// 非法键，返回空字符串
}

bool Util::is_valid_length_expression(const std::string& input)
{
    if(input.empty())
        return false;

    // 使用正则表达式验证格式
    // 格式: 数字 (可包含小数) + 单位 (cm, mm, m)
    std::regex lengthRegex(R"(^-?(\d+(\.\d*)?|\.\d+)(cm|mm|m)?$)");
    return std::regex_match(input, lengthRegex);
}

std::pair<float, std::string> Util::extract_value_and_unit(const std::string& input)
{
    if(input.empty())
    {
        return std::make_pair(0.0f, "");
    }

    std::regex lengthRegex(R"(^(-?\d+(\.\d*)?|\.\d+)(cm|mm|m)?$)");
    std::smatch match;

    if(std::regex_match(input, match, lengthRegex))
    {
        float length = std::stof(match[1].str());
        std::string unit = match[3].str();
        return std::make_pair(length, unit);
    }

    return std::make_pair(0.0f, "");
}

std::string Util::cut_string_with_two_decimals(float value)
{
    value = std::round(value * 100) / 100;
    std::string str = std::to_string(value);

    size_t dot_pos = str.find('.');
    if(dot_pos != std::string::npos && dot_pos + 3 < str.length())
    {
        str = str.substr(0, dot_pos + 3);
    }
    return str;
}
}// namespace sketch