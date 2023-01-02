#include "mcvulkan/window.hpp"
#include "mcvulkan/logger.hpp"
#include "mcvulkan/global.hpp"
#include "mcvulkan/device.hpp"
#include <vulkan/vulkan.h>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <vector>
#include <cstring>

static void game();
static bool init_vulkan(VkInstance &instance 
#ifndef NDEBUG 
    ,VkDebugUtilsMessengerEXT &debug_messenger
#endif
, Device::LogicalDevice &device
);

#ifndef NDEBUG
    static constexpr std::array validation_layers {
        "VK_LAYER_KHRONOS_validation"
    };
    static bool has_validation_layer_support();
    static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                            VkDebugUtilsMessageTypeFlagsEXT,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                                                            void *) noexcept;
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                                 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                                 const VkAllocationCallbacks* pAllocator, 
                                                 VkDebugUtilsMessengerEXT* pDebugMessenger) noexcept;
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                              VkDebugUtilsMessengerEXT messenger, 
                                              const VkAllocationCallbacks *pAllocator) noexcept;
#endif
static std::vector<const char*> get_required_extensions();

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
    #ifndef NDEBUG
        VkDebugUtilsMessengerEXT debug_messenger {};
    #endif
    VkInstance instance {};
    // Introduce another scope to avoid problems with de-allocating
    // the logical device after the instance has been destroyed
    {
        Device::LogicalDevice logical_device {};

        #ifndef NDEBUG 
            if (!init_vulkan(instance, debug_messenger, logical_device))
                Logger::fatal_error("Failed to initialize vulkan instance");
        #else
            if (!init_vulkan(instance, device))
                Logger::fatal_error("Failed to initialize vulkan instance");
        #endif


        while (!glfwWindowShouldClose(window.get())) {
            glfwPollEvents();
        }
    }
    #ifndef NDEBUG
        DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    #endif

    vkDestroyInstance(instance, nullptr);
}

static bool init_vulkan(VkInstance &instance
#ifndef NDEBUG
    ,VkDebugUtilsMessengerEXT &debug_messenger
#endif
, Device::LogicalDevice &device
)
{
    static constexpr VkApplicationInfo app_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Minecraft",
        .applicationVersion = VK_API_VERSION_1_0,
        .pEngineName = "No Engine",
        .engineVersion = VK_API_VERSION_1_0,
        .apiVersion = VK_API_VERSION_1_0,
    };

    const auto glfw_extensions {get_required_extensions()};

    #ifdef NDEBUG
        const VkInstanceCreateInfo glb_info {
            .enabledLayerCount = 0,
            .enabledExtensionCount = count,
            .ppEnabledExtensionNames = glfw_extensions
        };

        const VkResult result {vkCreateInstance(&glb_info, nullptr, &instance)};

        if (result != VK_SUCCESS)
            return false;
        return true;
    #else
        VkDebugUtilsMessengerCreateInfoEXT glb_debug_info {};
        {
            if (!has_validation_layer_support()) {
                Logger::fatal_error("Validation layers not available");
                return false;
            }


            glb_debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            glb_debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            glb_debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            glb_debug_info.pfnUserCallback = vk_debug_callback;
            glb_debug_info.pUserData = nullptr;


            VkInstanceCreateInfo glb_info {};
            glb_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            glb_info.enabledLayerCount = validation_layers.size();
            glb_info.ppEnabledLayerNames = validation_layers.data();
            glb_info.enabledExtensionCount = static_cast<u32>(glfw_extensions.size());
            glb_info.ppEnabledExtensionNames = glfw_extensions.data();
            glb_info.pApplicationInfo = &app_info;
            glb_info.pNext = &glb_debug_info;
            const VkResult result {vkCreateInstance(&glb_info, nullptr, &instance)};

            if (result != VK_SUCCESS)
                return false;
            
            const VkResult debug_result {CreateDebugUtilsMessengerEXT(instance, &glb_debug_info, nullptr, &debug_messenger)};

            if (debug_result != VK_SUCCESS) {
                Logger::fatal_error("Failed to setup debug messenger");
                return false;
            }

            const Device::DeviceInfo device_info {Device::select_physical_device(instance)};

            device = Device::LogicalDevice{device_info};
            
            return true;
        }
    #endif

}


static std::vector<const char*> get_required_extensions()
{
    u32 count {};
    const auto glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions {glfw_extensions, glfw_extensions + count};

    // Add the debug messenger extension (if running on a debug build)
    if constexpr (Global::IS_DEBUG_BUILD)
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    
    return extensions;
}

#ifndef NDEBUG
    static bool has_validation_layer_support()
    {
        u32 count {};
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

    // This is just a wrapper around 'vkCreateDebugUtilsMessengerEXT'
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                                 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                                 const VkAllocationCallbacks* pAllocator, 
                                                 VkDebugUtilsMessengerEXT* pDebugMessenger) noexcept
    {
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // This is just a wrapper around 'vkDestroyDebugUtilsMessengerEXT'
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                              VkDebugUtilsMessengerEXT messenger, 
                                              const VkAllocationCallbacks *pAllocator) noexcept
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr)
            return func(instance, messenger, pAllocator);
        Logger::error("Failed to load 'vkDestroyDebugUtilsMessengerExt' address");
    }

    // Vulkan debugger callback
    static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                            VkDebugUtilsMessageTypeFlagsEXT,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                                                            void *) noexcept
    {
        // call the appropriate logging function according to the severity level
        switch (severity) {
            default:
                Logger::error("Unknown severity level");
                Logger::error(p_callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: // diagnostic message
                Logger::diagnostic(p_callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: // informational message
                Logger::info(p_callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: // warning (very likely about a bug)
                Logger::warning(p_callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: // error
                Logger::error(p_callback_data->pMessage);
                break;
        }


        /* The callback returns a boolean that indicates if the Vulkan call 
           that triggered the validation layer message should be aborted. If 
           the callback returns true, then the call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT error. 
           So just leave it as VK_FALSE */
        return VK_FALSE;

    }
#endif

