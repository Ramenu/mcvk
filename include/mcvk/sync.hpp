#ifndef MCVK_SYNC_HPP
#define MCVK_SYNC_HPP

#include <vulkan/vulkan_core.h>

namespace Sync
{
    extern VkSemaphore create_semaphore(VkDevice device) noexcept;
    extern VkFence create_fence(VkDevice device, bool signal_on = true) noexcept;
}

#endif // MCVK_SYNC_HPP