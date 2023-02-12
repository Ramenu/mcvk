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

#include "mcvk/command.hpp"
#include "mcvk/device.hpp"
#include "mcvk/swapchain.hpp"
#include "mcvk/pipeline.hpp"
#include "mcvk/sync.hpp"
#ifndef NDEBUG
    #include <string>
#endif
#include <array>

static inline void setup_command_pool(const Device::LogicalDevice &device,
                                      u32 queue_family_index,
                                      VkCommandPool &command_pool) noexcept
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        assert(device.get() != VK_NULL_HANDLE);
    }
    VkCommandPoolCreateInfo pool_info {};

    // Hint that command buffers are rerecorded with new commands very often
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Allow command buffers to be rerecorded individually
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // We will execute command buffers on the graphics queue
    pool_info.queueFamilyIndex = queue_family_index; 

    if (vkCreateCommandPool(device.get(), &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
        Logger::fatal_error("Failed to create command pool");
    }

    #ifndef NDEBUG
        const std::string msg = std::string{"Allocated 'VkCommandPool' from device handle: '"} + 
                                device.device_name() + 
                                "'";
        Logger::info(msg.c_str());
    #endif
}

static inline void setup_command_buffer(const Device::LogicalDevice &device,
                                        const VkCommandPool &command_pool,
                                        VkCommandBuffer &command_buffer) noexcept
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        assert(device.get() != VK_NULL_HANDLE);
        assert(command_pool != VK_NULL_HANDLE);
    }
    VkCommandBufferAllocateInfo cmd_buffer_info {};
    cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buffer_info.commandPool = command_pool;

    // specifies if the allocated command buffers are primary or secondary command buffers:
    // primary buffers can be submitted to queue for execution, but cannot be called from other command buffers
    // secondary buffers cannot be submitted directly, but can be called from primary command buffers
    cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    cmd_buffer_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device.get(), &cmd_buffer_info, &command_buffer) != VK_SUCCESS) {
        Logger::fatal_error("Failed to allocate command buffer");
    }

    #ifndef NDEBUG
        const std::string msg = std::string{"Allocated 'VkCommandBuffer' from device handle: '"} + 
                                device.device_name() + 
                                "'" ;
        Logger::info(msg.c_str());
    #endif
}

Command::Command(const Device::LogicalDevice &device,
                 u32 queue_family_index) noexcept
    : device_handle {device.get()},
      render_finished_semaphore {Sync::create_semaphore(device.get())}
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        assert(device_handle != VK_NULL_HANDLE);
        assert(render_finished_semaphore != VK_NULL_HANDLE);
    }
    setup_command_pool(device, queue_family_index, this->command_pool);
    setup_command_buffer(device, this->command_pool, this->command_buffer);
}

void Command::record(const u32 swapchain_img_index, 
                     const Swapchain &swapchain,
                     const Pipeline &pipeline) noexcept
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        assert(this->device_handle != VK_NULL_HANDLE);
        assert(this->command_buffer != VK_NULL_HANDLE);
        assert(this->command_pool != VK_NULL_HANDLE);
        assert(swapchain_img_index < swapchain.size());
    }
    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // specifies how we are going to use the command buffer
    begin_info.flags = 0; // optional

    // only relevant for secondary command buffers. It specifies which state to inherit from the 
    // calling primary command buffer
    begin_info.pInheritanceInfo = nullptr; // optional

    if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS) {
        Logger::fatal_error("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo render_pass_begin_info {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    // specify the render pass
    render_pass_begin_info.renderPass = pipeline.get_renderpass();

    // bind the framebuffer to the image we want to draw to
    render_pass_begin_info.framebuffer = swapchain.framebuffer_at(swapchain_img_index);

    // define the size of the render area. The render area defines where the shader loads 
    // and stores will take place
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = swapchain.get_extent();

    // clear the framebuffer to black
    static constexpr VkClearValue clear_color {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    // Begin render pass
    // 'VK_SUBPASS_CONTENTS_INLINE' specifies that the secondary command buffers will not be executed
    // and the render pass commands will be embedded in the primary command buffer

    // 'VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS' specifies that the render pass commands will be
    // executed from secondary command buffers
    vkCmdBeginRenderPass(this->command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    // bind the graphics pipeline
    vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_pipeline());

    // since we assigned a dynamic viewport and scissor state, we have to specify their values in the command
    // buffer at draw time.

    // set viewport
    const VkViewport viewport {
        .x = 0.0f, 
        .y = 0.0f, 
        .width = static_cast<float>(swapchain.get_extent().width), 
        .height = static_cast<float>(swapchain.get_extent().height),
        .minDepth = 0.0f, 
        .maxDepth = 1.0f 
    };
    vkCmdSetViewport(this->command_buffer, 0, 1, &viewport);

    // set scissor
    const VkRect2D scissor {
        .offset = {0, 0}, 
        .extent = swapchain.get_extent()
    };
    vkCmdSetScissor(this->command_buffer, 0, 1, &scissor);

    // now we can draw!
    static constexpr usize VERTICES_TO_DRAW = 3;
    static constexpr usize INSTANCES_COUNT = 1;
    static constexpr usize FIRST_VERTEX = 0;
    static constexpr usize FIRST_INSTANCE = 0;
    vkCmdDraw(this->command_buffer, VERTICES_TO_DRAW, INSTANCES_COUNT, FIRST_VERTEX, FIRST_INSTANCE);

    // End render pass
    vkCmdEndRenderPass(this->command_buffer);

    // Finish recording command buffer
    if (vkEndCommandBuffer(this->command_buffer) != VK_SUCCESS) {
        Logger::fatal_error("Failed to record command buffer");
    }
}

void Command::submit(const Swapchain &swapchain,
                     const Device::LogicalDevice &device,
                     const VkFence in_flight_fence,
                     u32 swapchain_img_index) const noexcept
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        assert(this->device_handle != VK_NULL_HANDLE);
        assert(this->command_buffer != VK_NULL_HANDLE);
        assert(this->command_pool != VK_NULL_HANDLE);
        assert(this->render_finished_semaphore != VK_NULL_HANDLE);
    }

    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    static constexpr std::array<VkPipelineStageFlags, 1> wait_stages {
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT // don't begin render pass until the image is available
    };

    // specify which semaphores to wait on before the command buffer starts execution and which
    // stage(s) of the pipeline to wait at. We do not want to write colors to the image until
    // it becomes available.
    submit_info.waitSemaphoreCount = wait_stages.size(); 
    submit_info.pWaitSemaphores = &swapchain.image_available();
    submit_info.pWaitDstStageMask = wait_stages.data();


    // specify command buffers to submit for execution
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &this->command_buffer;

    // specify which semaphores to signal once the command buffer has finished execution
    const std::array signal_semaphores {
        this->render_finished_semaphore
    };
    submit_info.signalSemaphoreCount = signal_semaphores.size();
    submit_info.pSignalSemaphores = signal_semaphores.data();

    // fence will be signaled once the command buffer has finished execution. This allows
    // us to know when it is safe for the command buffer to be reused. Then on the next frame
    // the CPU will wait for the command buffer to finish executing before it records new
    // commands to it
    if (vkQueueSubmit(device.get_graphics_queue(), 1, &submit_info, in_flight_fence) != VK_SUCCESS) {
        Logger::fatal_error("Failed to submit draw command buffer");
    }

    // Submit result back to swapchain so it can display it on the screen
    VkPresentInfoKHR present_info {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // specify which semaphores to wait on before presentation can happen. Since we want
    // to wait on the command buffer to finish executing, we use the semaphores
    // that were signaled when the command buffer finished execution (in this case,
    // 'signal_semaphores')
    present_info.waitSemaphoreCount = signal_semaphores.size();
    present_info.pWaitSemaphores = signal_semaphores.data();

    // Specify the swapchain to present images to, and the index of the image for each swapchain.
    // This will almost always be 1
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain.get();
    present_info.pImageIndices = &swapchain_img_index;

    // Submit the request to present an image to the swapchain
    if (vkQueuePresentKHR(device.get_presentation_queue(), &present_info) != VK_SUCCESS) {
        Logger::error("Failed to present swapchain image");
    }


}
