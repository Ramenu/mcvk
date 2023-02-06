/**
 * This file is part of mcvk.
 * 
 * mcvk is free software: you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License as published by the Free Software Foundation, either version 
 * 3 of the License, or (at your option) any later version.
 * 
 * mcvk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with mcvk. If 
 * not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cstdlib>
#include <mutex>
#include "mcvk/utils/color.h"
#include "mcvk/logger.hpp"

namespace Logger
{

    static std::mutex mtx;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg)
    [[noreturn]] void fatal_error(const char *msg) noexcept
    {
        // Need a lock here because exit is not thread-safe, it uses a
        // global variable which is not protected so a race condition can
        // occur. 
        std::lock_guard lock {mtx};
        fprintf(stderr, COLOR_BOLDRED "FATAL ERROR: " COLOR_RESET "%s\n", msg);
        exit(EXIT_FAILURE);
    }

    void error(const char *msg) noexcept 
    {
        fprintf(stderr, COLOR_RED "ERROR: " COLOR_RESET "%s\n", msg);
    }

    #ifndef NDEBUG
        void diagnostic(const char *msg) noexcept
        {
            fprintf(stdout, COLOR_MAGENTA "DIAGNOSTIC: " COLOR_RESET "%s\n", msg);
        }

        void info(const char *msg) noexcept
        {
            fprintf(stdout, COLOR_MAGENTA "INFO: " COLOR_RESET "%s\n", msg);
        }

        void warning(const char *msg) noexcept
        {
            fprintf(stderr, COLOR_YELLOW "WARNING: " COLOR_RESET "%s\n", msg);
        }
    #endif
    // NOLINTEND(cppcoreguidelines-pro-type-vararg)
}
