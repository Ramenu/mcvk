#include "mcvulkan/window.hpp"
#include "mcvulkan/logger.hpp"
#include "mcvulkan/global.hpp"
#include <vulkan/vulkan.h>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <vector>
#include <string.h>

static void game();
static bool init_vulkan(VkInstance &instance);
#ifndef NDEBUG
    static constexpr std::array validation_layers {
        "VK_LAYER_KHRONOS_validation"
    };
    static bool has_validation_layer_support();
#endif

int main() 
{
    glfwInit();
    game();
    glfwTerminate();
    return 0;
}


static void game()
{
    static constexpr unsigned WIDTH {500}, HEIGHT {500};
    Window window {WIDTH, HEIGHT, "Minecraft"};
    VkInstance instance {};

    if (!init_vulkan(instance))
        Logger::fatal_error("Failed to initialize vulkan instance");

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();
    }

    vkDestroyInstance(instance, nullptr);
}

static bool init_vulkan(VkInstance &instance)
{
    #if 0
    static constexpr auto VK_API_VERSION = VK_MAKE_API_VERSION(VK_VERSION_1_0, 1, 0, 0);
    static constexpr VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Minecraft",
        .applicationVersion = VK_API_VERSION,
        .pEngineName = "No Engine",
        .engineVersion = VK_API_VERSION,
        .apiVersion = VK_API_VERSION_1_0
    };
    #endif

    uint32_t count {};
    const auto glfw_extensions {glfwGetRequiredInstanceExtensions(&count)};

    #ifdef NDEBUG
        const VkInstanceCreateInfo glb_info {
            .enabledLayerCount = 0,
            .enabledExtensionCount = count,
            .ppEnabledExtensionNames = glfw_extensions
        };
    #else
        if (!has_validation_layer_support()) {
            Logger::fatal_error("Validation layers not available");
            return false;
        }

        VkInstanceCreateInfo glb_info {};
        glb_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        glb_info.enabledLayerCount = validation_layers.size();
        glb_info.ppEnabledLayerNames = validation_layers.data();
        glb_info.enabledExtensionCount = count;
        glb_info.ppEnabledExtensionNames = glfw_extensions;
    #endif

    const VkResult result {vkCreateInstance(&glb_info, nullptr, &instance)};

    if (result != VK_SUCCESS)
        return false;
    return true;
}

#ifndef NDEBUG
    static bool has_validation_layer_support()
    {
        uint32_t count {};
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        std::vector<VkLayerProperties> layer_properties (count);
        vkEnumerateInstanceLayerProperties(&count, layer_properties.data());

        for (auto validation_layer : validation_layers) {
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