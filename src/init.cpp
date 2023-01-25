#include "mcvk/window.hpp"
#include "mcvk/logger.hpp"
#include "mcvk/global.hpp"
#include "mcvk/device.hpp"
#include "mcvk/vkcomponents.hpp"
#include "mcvk/validationlayers.hpp"
#include "mcvk/swapchain.hpp"
#include <vulkan/vulkan.h>
#include <cstring>
#include <cstdlib>
#include <vector>


static void game();
static void init_vulkan(const VkComponents &components, 
                        Device::LogicalDevice &device, 
                        Swapchain &swapchain,
                        GLFWwindow *window,
                        std::vector<VkImageView> &swapchain_image_views) noexcept;
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

    // this stores a image view for every image in the swapchain
    std::vector<VkImageView> swapchain_image_views;

    // Initialize base vulkan instance, setting up physical/logical devices, debug messengers, swapchain, etc.
    init_vulkan(components, device, swapchain, window.self, swapchain_image_views);

    while (!glfwWindowShouldClose(window.self)) [[likely]] {
        glfwPollEvents();
    }

    for (auto &image_view : swapchain_image_views) {
        if (image_view != VK_NULL_HANDLE)
            vkDestroyImageView(device.get(), image_view, nullptr);
        #ifndef NDEBUG
            else
                Logger::fatal_error("Failed to de-allocate 'VkImageView'. 'VkImageView' is null");
        #endif
    }
}

// Components must be initialized before this is called, as it can affect the physical device selection
static void init_vulkan(const VkComponents &components, 
                        Device::LogicalDevice &device, 
                        Swapchain &swapchain,
                        GLFWwindow *window,
                        std::vector<VkImageView> &swapchain_image_views) noexcept
{
    const Device::DeviceInfo device_info {Device::select_physical_device(components, window)};
    device = Device::LogicalDevice{device_info};
    swapchain = Swapchain{device_info.device, 
                          components.get_surface(),
                          window,
                          device_info.queue_family_indices, 
                          device.get()};

    swapchain_image_views.resize(swapchain.size());
    for (usize i = 0; i < swapchain.size(); ++i) {
        VkImageViewCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = swapchain[i];
        info.format = swapchain.get_format();

        // specify how data should be interpreted (in this case, as a 2D image)
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;

        // specify color channels (using default)
        info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // subresourceRange describes the properties of the image and which part of the image
        // should be accessed
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.get(), &info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS)
            Logger::fatal_error(std::string{"Failed to create image view at index " + std::to_string(i)}.c_str());
    }
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
