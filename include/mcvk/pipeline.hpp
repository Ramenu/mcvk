#ifndef MCVK_PIPELINE_HPP
#define MCVK_PIPELINE_HPP

#include <vulkan/vulkan_core.h>  // for VK_NULL_HANDLE, vkDestroyPipelineLayout
#include <cassert>               // for assert
#include "global.hpp"            // for DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT
#include "logger.hpp"            // for info
#include "shader.hpp"            // for Shader

namespace Device { class LogicalDevice; }

class Pipeline
{
    private:
        VkPipelineLayout pipeline_layout {VK_NULL_HANDLE};
        VkPipeline pipeline {VK_NULL_HANDLE};
        VkRenderPass renderpass {VK_NULL_HANDLE};
        Shader shader {};
        const VkDevice device_handle {VK_NULL_HANDLE};
        void init_renderpass(VkFormat swapchain_image_format) noexcept;
    public:
        Pipeline(const Device::LogicalDevice &device, VkFormat swapchain_image_format) noexcept;
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(Pipeline)
        ~Pipeline() noexcept
        {
            if constexpr (Global::IS_DEBUG_BUILD) {
                assert(device_handle != VK_NULL_HANDLE);
                assert(pipeline_layout != VK_NULL_HANDLE);
                assert(pipeline != VK_NULL_HANDLE);
                assert(renderpass != VK_NULL_HANDLE);
                Logger::info("De-allocating 'VkRenderPass'");
                Logger::info("De-allocating 'VkPipelineLayout'");
                Logger::info("De-allocating 'VkPipeline'");
            }
            vkDestroyPipeline(device_handle, pipeline, nullptr);
            vkDestroyPipelineLayout(device_handle, pipeline_layout, nullptr);
            vkDestroyRenderPass(device_handle, renderpass, nullptr);
        }
        constexpr const auto &get_renderpass() const noexcept { return renderpass; }
        constexpr const auto &get_pipeline() const noexcept { return pipeline; }

};

#endif // MCVK_PIPELINE_HPP