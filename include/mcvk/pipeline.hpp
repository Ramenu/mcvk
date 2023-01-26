#ifndef MCVK_PIPELINE_HPP
#define MCVK_PIPELINE_HPP

#include <vulkan/vulkan.h>
#include "global.hpp"
#include "logger.hpp"
#include <cassert>

class Pipeline
{
    private:
        VkPipelineLayout pipeline_layout {VK_NULL_HANDLE};
        const VkDevice device_handle {VK_NULL_HANDLE};
    public:
        Pipeline(VkDevice device_handle) noexcept;
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(Pipeline)
        ~Pipeline() noexcept
        {
            if constexpr (Global::IS_DEBUG_BUILD) {
                assert(device_handle != VK_NULL_HANDLE);
                assert(pipeline_layout != VK_NULL_HANDLE);
                Logger::info("De-allocating 'VkPipelineLayout'");
            }
            vkDestroyPipelineLayout(device_handle, pipeline_layout, nullptr);
        }

};

#endif // MCVK_PIPELINE_HPP