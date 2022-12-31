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
        fprintf(stderr, COLOR_BOLDRED "ERROR: " COLOR_RESET "%s\n", msg);
    }
}