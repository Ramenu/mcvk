#include "mcvulkan/queue.hpp"
#include <vector>
#include "mcvulkan/logger.hpp"

namespace Queue
{

    // Check which queue families are supported by the device
    QueueFamilyIndices::QueueFamilyIndices(const Device::PhysicalDeviceInfo device, VkSurfaceKHR surface) noexcept
    {
        u32 count {};

        vkGetPhysicalDeviceQueueFamilyProperties(device.self, &count, nullptr);

        if (count == 0) {
            #ifndef NDEBUG
                const auto count_zero_msg = std::string{device.name} + " does not support any queue families";
                Logger::info(count_zero_msg.c_str());
            #endif
            return;
        }
        std::vector<VkQueueFamilyProperties> families (count);
        vkGetPhysicalDeviceQueueFamilyProperties(device.self, &count, families.data());


        for (u32 i {}; i < families.size(); ++i) {
            VkBool32 device_has_presentation_queue = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device.self, i, surface, &device_has_presentation_queue);

            if (device_has_presentation_queue) {
                this->set(FamilyIndex::PresentationQueueIndex, i);
                #ifndef NDEBUG
                    const auto msg = std::string{"Found presentation queue family on device "} + device.name;
                    Logger::info(msg.c_str());
                #endif
                this->flags = static_cast<IndexFlags>(static_cast<usize>(this->flags)|static_cast<usize>(IndexFlags::PresentationQueueCompatible));
            }

            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                this->set(FamilyIndex::GraphicsQueueIndex, i);
                #ifndef NDEBUG
                    const auto msg = std::string{"Found graphics queue family on device "} + device.name;
                    Logger::info(msg.c_str());
                #endif
                this->flags = static_cast<IndexFlags>(static_cast<usize>(this->flags)|static_cast<usize>(IndexFlags::GraphicsQueueCompatible));
            }

            if (this->is_complete()) {
                #ifndef NDEBUG
                    const auto msg = std::string{"Found all required queue families on device "} + device.name;
                    Logger::info(msg.c_str());
                #endif
                break;
            }
        }
    }

}