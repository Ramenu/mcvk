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

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>       // for VkLayerProperties, vkEnumerateI...
#include <array>                      // for array
#include <cstring>                    // for strcmp
#include <vector>                     // for vector

#include "mcvk/device.hpp"            // for select_physical_device, Logical...
#include "mcvk/logger.hpp"            // for fatal_error
#include "mcvk/swapchain.hpp"         // for Swapchain
#include "mcvk/types.hpp"             // for u32
#include "mcvk/validationlayers.hpp"  // for VALIDATION_LAYERS
#include "mcvk/vkcomponents.hpp"      // for VkComponents
#include "mcvk/window.hpp"            // for Window, glfwInit, glfwPollEvents
#include "mcvk/pipeline.hpp"          // for Pipeline
#include "mcvk/command.hpp"
#include "mcvk/queue.hpp"
#include "mcvk/sync.hpp"
#include "mcvk/global.hpp"


static constexpr auto NUMBER_OF_FENCES = 1;
static inline void game() noexcept;
static inline void init_vulkan(const VkComponents &components, 
                        Device::LogicalDevice &device, 
                        Swapchain &swapchain,
                        GLFWwindow *window,
                        Queue::QueueFamilyIndices &) noexcept;
static inline void draw_frame(const Device::LogicalDevice &,
                              VkFence,
                              const Swapchain &,
                                    Command &,
                              const Pipeline &) noexcept;
static void wait_for_prev_frame_to_finish(const Device::LogicalDevice &,
                                          const VkFence &) noexcept;
#ifndef NDEBUG
    static bool has_validation_layer_support() noexcept;
#endif


int main() noexcept
{
    // Before initializing the game, check if validation layers are supported
    // (only necessary for debug builds)
    #ifndef NDEBUG
        if (!has_validation_layer_support())
            Logger::fatal_error("Validation layers requested, but not available");
    #endif
    glfwInit();
    game();
    glfwTerminate();
    return 0;
}


static inline void game() noexcept
{
    static constexpr unsigned WIDTH {500}, HEIGHT {500};
    Window window {WIDTH, HEIGHT, "Minecraft"};

    #ifndef NDEBUG
        static constexpr bool USE_DEBUG_MESSENGER = true;
        VkComponents components {USE_DEBUG_MESSENGER, window.self};
    #else
        VkComponents components {window.self};
    #endif

    Device::LogicalDevice device;
    Swapchain swapchain {};
    Queue::QueueFamilyIndices queue_family_indices {};

    // Initialize base vulkan instance, setting up physical/logical devices, device queue families, debug messengers, swapchain, etc.
    init_vulkan(components, device, swapchain, window.self, queue_family_indices);

    // Initialize the graphics pipeline
    Pipeline pipeline {device, swapchain.get_format()};

    // Initialize the framebuffers in the swapchain, we do this now because the pipeline was only just now
    // initialized
    swapchain.initialize_framebuffers(pipeline.get_renderpass());

    // Create command buffer
    Command command {device, queue_family_indices.get(Queue::GraphicsQueueIndex)};

    // Create Vulkan synchronization primitives
    VkFence in_flight_fence {Sync::create_fence(device.get())};

    while (!glfwWindowShouldClose(window.self)) [[likely]] 
    {
        glfwPollEvents();
        draw_frame(device, in_flight_fence, swapchain, command, pipeline);
    }

    // Ensure no resource is being accessed by GPU before we de-allocate resources
    vkDeviceWaitIdle(device.get());
    
    vkDestroyFence(device.get(), in_flight_fence, nullptr);
}

static inline void draw_frame(const Device::LogicalDevice &device, 
                              const VkFence in_flight_fence,
                              const Swapchain &swapchain,
                                    Command &command,
                              const Pipeline &pipeline) noexcept
{
    // Wait for previous frame to finish
    wait_for_prev_frame_to_finish(device, in_flight_fence);

    // Reset the fence to the unsignaled state
    vkResetFences(device.get(), NUMBER_OF_FENCES, &in_flight_fence);

    // Acquire image from swapchain
    const auto image_index = swapchain.acquire_next_image();

    // Reset command buffer to begin recording
    command.reset();

    // Begin recording command buffer
    command.record(image_index, swapchain, pipeline);

    // Submit command buffer to graphics queue
    command.submit(swapchain, device, in_flight_fence, image_index);
}

static inline void wait_for_prev_frame_to_finish(const Device::LogicalDevice &device, 
                                                 const VkFence &in_flight_fence) noexcept
{
    static constexpr auto WAIT_FOR_ALL_FENCES = VK_TRUE;
    vkWaitForFences(device.get(), NUMBER_OF_FENCES, &in_flight_fence, WAIT_FOR_ALL_FENCES, Global::TIMEOUT_NS);
}

// Components must be initialized before this is called, as it can affect the physical device selection
static inline void init_vulkan(const VkComponents &components, 
                        Device::LogicalDevice &device, 
                        Swapchain &swapchain,
                        GLFWwindow *window,
                        Queue::QueueFamilyIndices &indices) noexcept
{
    const Device::DeviceInfo device_info {Device::select_physical_device(components, window)};
    device = Device::LogicalDevice{device_info};
    swapchain = Swapchain{device_info.device, 
                          components.get_surface(),
                          window,
                          device_info.queue_family_indices, 
                          device.get()};
    indices = device_info.queue_family_indices;
}

#ifndef NDEBUG
    static bool has_validation_layer_support() noexcept
    {
        u32 count {};
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        std::vector<VkLayerProperties> layer_properties (count);
        vkEnumerateInstanceLayerProperties(&count, layer_properties.data());

        for (const auto &validation_layer : VALIDATION_LAYERS) {
            bool found = false;
            for (const auto &properties : layer_properties) {
                if (strcmp(validation_layer, properties.layerName) == 0)  {
                    found = true;
                    break;
                }
            }

            if (!found)
                return false;
        }

        return true;  
    }
#endif
