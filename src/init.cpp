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


static void game();
static void init_vulkan(const VkComponents &components, 
                        Device::LogicalDevice &device, 
                        Swapchain &swapchain,
                        GLFWwindow *window) noexcept;
#ifndef NDEBUG
    static bool has_validation_layer_support() noexcept;
#endif

int main() 
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


static void game()
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

    // Initialize base vulkan instance, setting up physical/logical devices, debug messengers, swapchain, etc.
    init_vulkan(components, device, swapchain, window.self);

    // Initialize the graphics pipeline
    Pipeline pipeline {device.get(), swapchain.get_format()};

    while (!glfwWindowShouldClose(window.self)) [[likely]] 
    {
        glfwPollEvents();
    }
}

// Components must be initialized before this is called, as it can affect the physical device selection
static void init_vulkan(const VkComponents &components, 
                        Device::LogicalDevice &device, 
                        Swapchain &swapchain,
                        GLFWwindow *window) noexcept
{
    const Device::DeviceInfo device_info {Device::select_physical_device(components, window)};
    device = Device::LogicalDevice{device_info};
    swapchain = Swapchain{device_info.device, 
                          components.get_surface(),
                          window,
                          device_info.queue_family_indices, 
                          device.get()};
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
