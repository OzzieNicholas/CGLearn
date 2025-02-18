#pragma once

#include "sketch/types.h"
#include <stringzilla/stringzilla.hpp>

namespace sketch::render
{

template<typename char_type, typename allocator_type = std::allocator<char_type>>
using basic_string = ashvardanian::stringzilla::basic_string<char_type, allocator_type>;

template<typename char_type> using basic_string_view = ashvardanian::stringzilla::basic_string_slice<char_type>;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using string_view = basic_string_view<char const>;
using wstring_view = basic_string_view<wchar_t const>;

using mat2c = Eigen::Matrix<float, 2, 2, Eigen::ColMajor>;
using mat3c = Eigen::Matrix<float, 3, 3, Eigen::ColMajor>;
using mat4c = Eigen::Matrix<float, 4, 4, Eigen::ColMajor>;

}// namespace sketch::render

#ifdef __cpp_lib_format
#include <format>
template<> struct std::formatter<sketch::render::string>
{
    constexpr auto parse(std::format_parse_context& ctx) const { return ctx.begin(); }

    auto format(const sketch::render::string& str, std::format_context& ctx) const { return std::format_to(ctx.out(), "{}", str.c_str()); }
};

#else

#include <fmt/format.h>

template<typename char_type>
struct fmt::formatter<sketch::render::basic_string<char_type>, char_type> : fmt::formatter<fmt::basic_string_view<char_type>, char_type>
{
    template<typename FormatContext> constexpr auto format(const sketch::render::string& str, FormatContext& ctx) const
    {
        return fmt::formatter<fmt::basic_string_view<char_type>, char_type>::format(basic_string_view<char_type>(str.data(), str.length()), ctx);
    }
};

template<typename char_type>
struct fmt::formatter<sketch::render::basic_string_view<char_type>, char_type> : fmt::formatter<fmt::basic_string_view<char_type>, char_type>
{
    template<typename FormatContext> constexpr auto format(const sketch::render::string& str, FormatContext& ctx) const
    {
        return fmt::formatter<fmt::basic_string_view<char_type>, char_type>::format(basic_string_view<char_type>(str.data(), str.length()), ctx);
    }
};

template<typename scalar, int rows, int cols, int option> struct fmt::formatter<Eigen::Matrix<scalar, rows, cols, option>>
{
    constexpr auto parse(fmt::format_parse_context& ctx) const { return ctx.begin(); }

    template<typename FormatContext> auto format(const Eigen::Matrix<scalar, rows, cols, option>& matrix, FormatContext& ctx) const
    {
        std::stringstream ss;
        ss << matrix;
        return format_to(ctx.out(), "{}", ss.str());
    }
};

#endif
