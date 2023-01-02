#ifndef MCVULKAN_QUEUE_HPP
#define MCVULKAN_QUEUE_HPP

#include <vulkan/vulkan.h>
#include <optional>
#include "mcvulkan/types.hpp"

namespace Queue
{
    class QueueFamilyIndices
    {
        public:
            std::optional<u32> graphics {};
            auto constexpr is_complete() const { return graphics.has_value(); }
    };

    extern QueueFamilyIndices find_queue_families(VkPhysicalDevice device
    #ifndef NDEBUG
        , const char *device_name
    #endif
    ) noexcept;
}

#endif // MCVULKAN_QUEUE_HPP