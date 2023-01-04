#ifndef MCVULKAN_QUEUE_HPP
#define MCVULKAN_QUEUE_HPP

#include <optional>
#include "mcvulkan/types.hpp"
#include "mcvulkan/physicaldeviceinfo.hpp"

namespace Queue
{
    class QueueFamilyIndices
    {
        public:
            std::optional<u32> graphics {};
            std::optional<u32> presentation {};
            auto constexpr is_complete() const { return graphics.has_value() && presentation.has_value(); }
    };

    extern QueueFamilyIndices find_queue_families(Device::PhysicalDeviceInfo physical_device, VkSurfaceKHR surface) noexcept;
}

#endif // MCVULKAN_QUEUE_HPP