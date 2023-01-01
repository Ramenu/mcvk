#ifndef MCVULKAN_DEVICE_HPP
#define MCVULKAN_DEVICE_HPP

#include <vulkan/vulkan.h>

namespace Device
{
    extern void select_physical_device(VkInstance instance) noexcept;
}

#endif // MCVULKAN_DEVICE_HPP
