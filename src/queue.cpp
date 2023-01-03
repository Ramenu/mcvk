#include "mcvulkan/queue.hpp"
#include <vector>
#include "mcvulkan/logger.hpp"
#include <string>

namespace Queue
{

    // Check which queue families are supported by the device
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface
    #ifndef NDEBUG
        , const char *device_name
    #endif
    ) noexcept
    {
        QueueFamilyIndices indices {};
        u32 count {};

        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        if (count == 0) {
            #ifndef NDEBUG
                const auto count_zero_msg = std::string{device_name} + " does not support any queue families";
                Logger::diagnostic(count_zero_msg.c_str());
            #endif
            return indices;
        }
        std::vector<VkQueueFamilyProperties> families (count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());


        for (u32 i {}; i < families.size(); ++i) {
            VkBool32 device_has_presentation_queue = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &device_has_presentation_queue);

            if (device_has_presentation_queue) {
                indices.presentation = i;
                #ifndef NDEBUG
                    const auto msg = std::string{"Found presentation queue family on device "} + device_name;
                    Logger::diagnostic(msg.c_str());
                #endif
            }

            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphics = i;
                #ifndef NDEBUG
                    const auto msg = std::string{"Found graphics queue family on device "} + device_name;
                    Logger::diagnostic(msg.c_str());
                #endif
            }

            if (indices.is_complete()) {
                #ifndef NDEBUG
                    const auto msg = std::string{"Found all required queue families on device "} + device_name;
                    Logger::diagnostic(msg.c_str());
                #endif
                break;
            }
        }

        return indices;

    }

}