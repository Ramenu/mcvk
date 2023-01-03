#include "mcvulkan/window.hpp"
#include "mcvulkan/logger.hpp"
#include "mcvulkan/global.hpp"
#include "mcvulkan/device.hpp"
#include "mcvulkan/vkcomponents.hpp"
#include "mcvulkan/validationlayers.hpp"
#include <vulkan/vulkan.h>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <vector>
#include <cstring>


static void game();
static void init_vulkan(const VkComponents &components, Device::LogicalDevice &device) noexcept;
#ifndef NDEBUG
    static bool has_validation_layer_support() noexcept;
#endif

int main() 
{
    // Before initializing the game, check if validation layers are supported
    // (only necessary for debug builds)
    #ifndef NDEBUG
        if (!has_validation_layer_support()) {
            Logger::fatal_error("Validation layers requested, but not available");
            return EXIT_FAILURE;
        }
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

    // Initialize base vulkan instance, setting up physical/logical devices, debug messengers, etc.
    init_vulkan(components, device);

    while (!glfwWindowShouldClose(window.self)) {
        glfwPollEvents();
    }

}

static void init_vulkan(const VkComponents &components, Device::LogicalDevice &device) noexcept
{
    const Device::DeviceInfo device_info {Device::select_physical_device(components)};
    device = Device::LogicalDevice{device_info};
}

static bool has_validation_layer_support() noexcept
{
    u32 count {};
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    std::vector<VkLayerProperties> layer_properties (count);
    vkEnumerateInstanceLayerProperties(&count, layer_properties.data());

    for (auto validation_layer : VALIDATION_LAYERS) {
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
