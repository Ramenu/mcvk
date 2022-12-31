#ifndef LOGGER_HPP
#define LOGGER_HPP

namespace Logger
{
    extern void fatal_error(const char *msg) noexcept;
    extern void error(const char *msg) noexcept;
}

#endif // LOGGER_HPP
