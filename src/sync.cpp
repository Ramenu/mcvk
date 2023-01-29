#include "mcvk/sync.hpp"
#include "mcvk/logger.hpp"
#include "mcvk/global.hpp"
#include <cassert>

namespace Sync
{
    VkSemaphore create_semaphore(VkDevice device) noexcept
    {
        if constexpr (Global::IS_DEBUG_BUILD)
           assert(device != VK_NULL_HANDLE && "Device is null"); 
        VkSemaphoreCreateInfo semaphore_info {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkSemaphore semaphore {VK_NULL_HANDLE};
        if (vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore) != VK_SUCCESS)
            Logger::fatal_error("Failed to create semaphore");

        return semaphore;
    }

    VkFence create_fence(VkDevice device, bool signal_on) noexcept
    {
        if constexpr (Global::IS_DEBUG_BUILD)
           assert(device != VK_NULL_HANDLE);
        VkFenceCreateInfo fence_info {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = signal_on ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        VkFence fence {VK_NULL_HANDLE};
        if (vkCreateFence(device, &fence_info, nullptr, &fence) != VK_SUCCESS)
            Logger::fatal_error("Failed to create fence");

        return fence;
    }

}