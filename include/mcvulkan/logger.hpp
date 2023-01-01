#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "types.hpp"

namespace Logger
{
    extern void fatal_error(const char *msg) noexcept;
    extern void error(const char *msg) noexcept;
    extern void diagnostic(const char *msg) noexcept;
    extern void info(const char *msg) noexcept;
    extern void warning(const char *msg) noexcept;
}

#endif // LOGGER_HPP
