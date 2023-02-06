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

#ifndef MCVK_GLOBAL_HPP
#define MCVK_GLOBAL_HPP

#include "mcvk/types.hpp"

#define DELETE_NON_MOVABLE_DEFAULT(T) \
T &operator=(T &&) = delete; \
T(T &&) = delete;

#define DELETE_NON_COPYABLE_DEFAULT(T) \
T &operator=(const T &) = delete; \
T(const T &) = delete;

#define DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(T) \
DELETE_NON_COPYABLE_DEFAULT(T) \
DELETE_NON_MOVABLE_DEFAULT(T)

namespace Global
{
    static constexpr auto TIMEOUT_NS = UINT64_MAX; // this value basically disables timeout
    consteval auto FLAG_SUM(usize max)
    {
        return (1 << (max - 1)) * 2 - 1;
    }
    // Should be used for comparsions between two values/objects.
    // Order should be:
    // $1 > $2: Greater
    // $1 = $2: Equal
    // $1 < $2: Less
    // If a comparsion fails for whatever reason, use Fail
    enum class Compare
    {
        Greater,
        Equal,
        Less,
        Fail
    };

    #ifndef NDEBUG
        static constexpr bool IS_DEBUG_BUILD = true;
    #else
        static constexpr bool IS_DEBUG_BUILD = false;
    #endif

}

#endif // MCVK_GLOBAL_HPP
