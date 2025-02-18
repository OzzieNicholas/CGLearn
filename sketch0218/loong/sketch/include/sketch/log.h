#pragma once

#include <nous_base/definitions.h>
#include <nous_base/log.h>

#ifdef __cpp_lib_format
#include <format>
#else
#include <fmt/format.h>
#endif

namespace sketch
{

#ifdef __cpp_lib_format
template<typename... Args> using format_string = std::format_string<Args...>;

template<typename... Args> N_FORCE_INLINE std::string format(format_string<Args...> fmt, Args&&... args)
{
    return std::format(fmt, std::forward<Args>(args)...);
}

#else

template<typename... Args> using format_string = fmt::format_string<Args...>;

template<typename... Args> N_FORCE_INLINE std::string format(format_string<Args...> fmt, Args&&... args)
{
    return fmt::format(fmt, std::forward<Args>(args)...);
}

#endif

template<typename... Args> N_FORCE_INLINE void log_info(format_string<Args...> msg, Args&&... args)
{
    nous::log_info(sketch::format(msg, std::forward<Args>(args)...));
}

template<typename... Args> N_FORCE_INLINE void log_warn(format_string<Args...> msg, Args&&... args)
{
    nous::log_warn(sketch::format(msg, std::forward<Args>(args)...));
}

template<typename... Args> N_FORCE_INLINE void log_error(format_string<Args...> msg, Args&&... args)
{
    nous::log_error(sketch::format(msg, std::forward<Args>(args)...));
}

template<typename... Args> N_FORCE_INLINE void log_debug(format_string<Args...> msg, Args&&... args)
{
    nous::log_debug(sketch::format(msg, std::forward<Args>(args)...));
}

template<typename... Args> N_FORCE_INLINE void log_trace(format_string<Args...> msg, Args&&... args)
{
    nous::log_trace(sketch::format(msg, std::forward<Args>(args)...));
}

template<typename... Args> N_FORCE_INLINE void log_off(format_string<Args...> msg, Args&&... args)
{
    nous::log(nous::LogLevel::Off, sketch::format(msg, std::forward<Args>(args)...));
}

}// namespace sketch
