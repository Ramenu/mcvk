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

#ifndef MCVK_COMMAND_HPP
#define MCVK_COMMAND_HPP

#include <vulkan/vulkan_core.h>  // for VK_NULL_HANDLE, vkDestroyCommandPool
#include <cassert>

#include "mcvk/global.hpp"
#include "mcvk/logger.hpp"
#include "mcvk/types.hpp"

namespace Queue {
enum FamilyIndex : u32;
}  // namespace Queue
namespace Device {
class LogicalDevice;
}  // namespace Device
class Swapchain;
class Pipeline;

class Command
{
    private:
        VkCommandPool command_pool {VK_NULL_HANDLE};
        VkCommandBuffer command_buffer {VK_NULL_HANDLE};
        const VkDevice device_handle {VK_NULL_HANDLE};
        VkSemaphore render_finished_semaphore {VK_NULL_HANDLE};
    public:
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(Command)
        Command(const Device::LogicalDevice &device, u32 queue_family_index) noexcept;
        ~Command() noexcept
        {
            if constexpr (Global::IS_DEBUG_BUILD) {
                assert(device_handle != VK_NULL_HANDLE);
                assert(command_pool != VK_NULL_HANDLE);
                assert(command_buffer != VK_NULL_HANDLE);
                assert(render_finished_semaphore != VK_NULL_HANDLE);
                Logger::info("De-allocating 'VkCommandPool'");
            }
            vkDestroySemaphore(device_handle, render_finished_semaphore, nullptr);
            vkDestroyCommandPool(device_handle, command_pool, nullptr);
        }
        // Writes the commands we want to execute to the command buffer
        void record(u32 swapchain_img_index, 
                    const Swapchain &swapchain,
                    const Pipeline &pipeline) noexcept;
        void reset() const noexcept
        {
            if constexpr (Global::IS_DEBUG_BUILD) {
                assert(device_handle != VK_NULL_HANDLE);
                assert(command_pool != VK_NULL_HANDLE);
            }
            vkResetCommandPool(device_handle, command_pool, 0);
        }
        void submit(const Swapchain &swapchain,
                    const Device::LogicalDevice &device,
                    VkFence in_flight_fence,
                    u32 swapchain_img_index) const noexcept;
};

#endif // MCVK_COMMAND_HPP
