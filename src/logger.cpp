#include <cstdio>
#include <cstdlib>
#include "mcvulkan/color.h"


namespace Logger
{
    void fatal_error(const char *msg) noexcept
    {
        fprintf(stderr, COLOR_BOLDRED "FATAL ERROR: " COLOR_RESET "%s\n", msg);
        exit(EXIT_FAILURE);
    }

    void error(const char *msg) noexcept 
    {
        fprintf(stderr, COLOR_RED "ERROR: " COLOR_RESET "%s\n", msg);
    }

    void diagnostic(const char *msg) noexcept
    {
        fprintf(stderr, COLOR_MAGENTA "DIAGNOSTIC: " COLOR_RESET "%s\n", msg);
    }

    void info(const char *msg) noexcept
    {
        fprintf(stderr, COLOR_MAGENTA "INFO: " COLOR_RESET "%s\n", msg);
    }

    void warning(const char *msg) noexcept
    {
        fprintf(stderr, COLOR_YELLOW "WARNING: " COLOR_RESET "%s\n", msg);
    }
}