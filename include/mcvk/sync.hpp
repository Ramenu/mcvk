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

#ifndef MCVK_SYNC_HPP
#define MCVK_SYNC_HPP

#include <vulkan/vulkan_core.h>

namespace Sync
{
    extern VkSemaphore create_semaphore(VkDevice device) noexcept;
    extern VkFence create_fence(VkDevice device, bool signal_on = true) noexcept;
}

#endif // MCVK_SYNC_HPP
