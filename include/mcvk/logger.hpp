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

#ifndef MCVK_LOGGER_HPP
#define MCVK_LOGGER_HPP

namespace Logger
{
    // for unrecoverable errors
    [[noreturn]] extern void fatal_error(const char *msg) noexcept;

    // for recoverable errors
    extern void error(const char *msg) noexcept;


    #ifndef NDEBUG
        // does not serve any purpose other than being used to differentiate logging 
        // and diagnostic messages in the vk debug messenger
        extern void diagnostic(const char *msg) noexcept;

        // for logging messages, debugging, etc.
        extern void info(const char *msg) noexcept;

        // for warnings
        extern void warning(const char *msg) noexcept;
        
    #endif

}

#endif // MCVK_LOGGER_HPP
