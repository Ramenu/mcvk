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

#include "mcvk/queue.hpp"
#include <string>                       // for operator+, allocator, string
#include <vector>                       // for vector
#include "mcvk/global.hpp"              // for IS_DEBUG_BUILD
#include "mcvk/logger.hpp"              // for info
#include "mcvk/physicaldeviceinfo.hpp"  // for PhysicalDeviceInfo
#include "mcvk/types.hpp"               // for u32

namespace Queue
{

    // Check which queue families are supported by the device
    QueueFamilyIndices::QueueFamilyIndices(const Device::PhysicalDeviceInfo device, VkSurfaceKHR surface) noexcept
    {
        u32 count {};

        vkGetPhysicalDeviceQueueFamilyProperties(device.self, &count, nullptr);

        if (count == 0) {
            if constexpr (Global::IS_DEBUG_BUILD) {
                const auto count_zero_msg = device.name + " does not support any queue families";
                Logger::info(count_zero_msg.c_str());
            }
            return;
        }
        std::vector<VkQueueFamilyProperties> families (count);
        vkGetPhysicalDeviceQueueFamilyProperties(device.self, &count, families.data());

        for (u32 i {}; i < families.size(); ++i) {
            VkBool32 device_has_presentation_queue = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device.self, i, surface, &device_has_presentation_queue);

            if (device_has_presentation_queue) {
                this->set(FamilyIndex::PresentationQueueIndex, i);
                if constexpr (Global::IS_DEBUG_BUILD) {
                    const auto msg = std::string{"Found presentation queue family on device "} + device.name;
                    Logger::info(msg.c_str());
                }
                this->flags = static_cast<IndexFlags>(this->flags|IndexFlags::PresentationQueueCompatible);
            }
            

            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                this->set(FamilyIndex::GraphicsQueueIndex, i);
                if constexpr (Global::IS_DEBUG_BUILD) {
                    const auto msg = std::string{"Found graphics queue family on device "} + device.name;
                    Logger::info(msg.c_str());
                }
                this->flags = static_cast<IndexFlags>(this->flags|IndexFlags::GraphicsQueueCompatible);
            }

            if (this->is_complete()) {
                if constexpr (Global::IS_DEBUG_BUILD) {
                    const auto msg = std::string{"Found all required queue families on device "} + device.name;
                    Logger::info(msg.c_str());
                }
                break;
            }
        }
    }

}
