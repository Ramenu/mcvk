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

#include "mcvk/swapchain.hpp"
#include <GLFW/glfw3.h>                 // for GLFWwindow, glfwGetFramebuffe...
#include <algorithm>                    // for clamp, find_if
#include <array>                        // for array
#include <iterator>                     // for end
#include <limits>                       // for numeric_limits
#include <string>                       // for operator+, char_traits, to_st...
#include "mcvk/device.hpp"              // for LogicalDevice
#include "mcvk/global.hpp"              // for IS_DEBUG_BUILD
#include "mcvk/physicaldeviceinfo.hpp"  // for PhysicalDeviceInfo
#include "mcvk/queue.hpp"               // for QueueFamilyIndices, FamilyIndex
#include "mcvk/types.hpp"               // for u32, usize
#include "mcvk/sync.hpp"


inline static VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats) noexcept;
inline static VkPresentModeKHR choose_swap_presentation_mode(const std::vector<VkPresentModeKHR> &presentation_modes) noexcept;
inline static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities,
                                                  GLFWwindow *window) noexcept;

Swapchain::Swapchain(const Device::PhysicalDeviceInfo physical_device, 
                     VkSurfaceKHR surface, 
                     GLFWwindow *window,
                     const Queue::QueueFamilyIndices &queue_family_indices,
                     VkDevice ddevice) noexcept
{
    VkSurfaceCapabilitiesKHR capabilities {};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device.self, surface, &capabilities) != VK_SUCCESS) {
        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"Failed to retrieve surface capabilities for device "} + physical_device.name;
            Logger::info(msg.c_str());
        }
        return;
    }

    std::vector<VkSurfaceFormatKHR> formats;
    u32 format_count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.self, surface, &format_count, nullptr);
    
    if (format_count != 0) {
        compatible_flag = static_cast<CompatibleFlag>(compatible_flag | CompatibleFlag::CompatibleWithSurfaceFormat);
        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"Found "} + std::to_string(format_count) + " surface formats for device " + physical_device.name;
            Logger::info(msg.c_str());
        }

        formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.self, surface, &format_count, formats.data());
    }
    else {
        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"No surface formats found for device "} + physical_device.name;
            Logger::info(msg.c_str());
        }
        return;
    }

    std::vector<VkPresentModeKHR> presentation_modes;
    u32 presentation_mode_count {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.self, surface, &presentation_mode_count, nullptr);

    if (presentation_mode_count != 0) {
        compatible_flag = static_cast<CompatibleFlag>(compatible_flag | CompatibleFlag::CompatibleWithPresentation);
        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"Found "} + std::to_string(presentation_mode_count) + " presentation modes for device " + physical_device.name;
            Logger::info(msg.c_str());
        }

        presentation_modes.resize(presentation_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.self, surface, &presentation_mode_count, presentation_modes.data());
    }
    else {
        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"No presentation modes found for device "} + physical_device.name;
            Logger::info(msg.c_str());
        }
        return;
    }

    // If the device is NULL, then the user probably wants to check if the device is compatible with the swapchain.
    // The compatible flag has been set, so it is unnecessary to proceed from here. Also check to make sure the
    // device is in fact being used.
    if (ddevice != VK_NULL_HANDLE && Device::LogicalDevice::device_is_in_use(ddevice)) {

        const auto swap_surface_format = choose_swap_surface_format(formats);
        const auto swap_presentation_mode = choose_swap_presentation_mode(presentation_modes);
        const auto available_images = std::clamp(capabilities.minImageCount + 1, capabilities.minImageCount, capabilities.maxImageCount);
        this->extent = choose_swap_extent(capabilities, window);

        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"Swapchain extent: "} + std::to_string(this->extent.width) + "x" + std::to_string(this->extent.height);
            Logger::info(msg.c_str());
        }

        VkSwapchainCreateInfoKHR swapchain_create_info {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = surface;
        swapchain_create_info.minImageCount = available_images;
        swapchain_create_info.imageColorSpace = swap_surface_format.colorSpace;
        swapchain_create_info.imageFormat = swap_surface_format.format;
        swapchain_create_info.imageExtent = this->extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // using swapchain for direct rendering, so use color attachment bit

        if (queue_family_indices.get(Queue::GraphicsQueueIndex) != queue_family_indices.get(Queue::PresentationQueueIndex)) {
            // Images can be used across multiple queue families without explicit ownership transfer
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.queueFamilyIndexCount = 2;
            swapchain_create_info.pQueueFamilyIndices = queue_family_indices.array().data();
        }
        else {
            // Ownership must be shared (i.e., transferred) to another queue family in order to be used. This offers the
            // best performance.
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

            swapchain_create_info.queueFamilyIndexCount = 0;
            swapchain_create_info.pQueueFamilyIndices = nullptr;
        }

        swapchain_create_info.preTransform = capabilities.currentTransform; // apply image transform if supported
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // do not blend with other windows
        swapchain_create_info.presentMode = swap_presentation_mode;
        swapchain_create_info.clipped = VK_TRUE; // don't care if another window gets in the way
        swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

        VkSwapchainKHR tmp {};
        if (vkCreateSwapchainKHR(ddevice, &swapchain_create_info, nullptr, &tmp) != VK_SUCCESS) {
            Logger::error("Failed to create swapchain");
            return;
        }
        #ifndef NDEBUG
            else {
                const auto msg = std::string{"Swapchain successfully created for device "} + physical_device.name;
                Logger::info(msg.c_str());
            }
        #endif

        // swapchain successfully created, so initialize the device, swapchain, and semaphore
        device = ddevice;
        swapchain = tmp;
        image_available_semaphore = Sync::create_semaphore(device);

        // Now get the handles of VkImage
        u32 image_count {};
        vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);

        if (image_count != 0) [[likely]] {
            images.resize(image_count);
            vkGetSwapchainImagesKHR(device, swapchain, &image_count, images.data());
        }
        #ifndef NDEBUG
            else {
                Logger::error("No images found for swapchain");
                return;
            }
        #endif

        this->format = swap_surface_format.format;

        this->initialize_image_views();

        // We don't initialize the framebuffers here, since the graphics pipeline hasn't been
        // initialized yet. To do that, call the initialize_framebuffers() method.
    }

}

inline static VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats) noexcept
{
    // We want to check for SRGB color space support, as it has more accurate
    // perceived colors and the standard color space for images.

    const auto found = std::find_if(formats.begin(), formats.end(), [](const auto &format) {
        return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    });

    // If found
    if (found != formats.end()) {
        if constexpr (Global::IS_DEBUG_BUILD)
            Logger::info("Found SRGB color space support for swapchain");
        return *found;
    }

    if constexpr (Global::IS_DEBUG_BUILD)
        Logger::info("No SRGB color space support for swapchain, using default");
    // If it's not available, just select the first format in the vector
    return formats[0];
}

inline static VkPresentModeKHR choose_swap_presentation_mode(const std::vector<VkPresentModeKHR> &presentation_modes) noexcept
{
    // Check for the most suitable presentation mode, 'VK_PRESENT_MODE_MAILBOX_KHR' is good for this as it renders
    // frames as fast as possible while also preventing tearing. Images that are queued are replaced with newer ones so
    // that there are no delays whatsoever. 
    const auto found = std::find_if(presentation_modes.begin(), presentation_modes.end(), [](auto mode){ 
        return mode == VK_PRESENT_MODE_MAILBOX_KHR;
    });

    // If found
    if (found != std::end(presentation_modes)) {
        if constexpr (Global::IS_DEBUG_BUILD)
            Logger::info("VK_PRESENT_MODE_MAILBOX_KHR support found for swapchain");
        return *found;
    }

    if constexpr (Global::IS_DEBUG_BUILD)
        Logger::info("VK_PRESENT_MODE_MAILBOX_KHR support not available, using VK_PRESENT_MODE_FIFO_KHR");

    // guaranteed to be available on every vulkan-supported device
    return VK_PRESENT_MODE_FIFO_KHR;
}

inline static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities,
                                                  GLFWwindow *window) noexcept
{

    if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
        return capabilities.currentExtent;
    
    // A value of '0xFFFFFFFF' means that the surface size will be determined based on the extent of
    // the swapchain targeting the surface. In which case we select the resolution fits the best between
    // the smallest and largest swapchain value extent.
    int width {}, height {};
    glfwGetFramebufferSize(window, &width, &height);

    // Values must be between the smallest and largest swapchain value extent supported.
    return {
        .width = std::clamp(static_cast<u32>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        .height = std::clamp(static_cast<u32>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void Swapchain::initialize_image_views() noexcept
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        static bool initialized = false;
        if (initialized)
            Logger::fatal_error("Swapchain image views already initialized");
        initialized = true;
    }

    image_views.resize(images.size());
    for (usize i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = images[i];
        info.format = this->format;

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

        if (vkCreateImageView(device, &info, nullptr, &image_views[i]) != VK_SUCCESS)
            Logger::fatal_error(std::string{"Failed to create image view at index " + std::to_string(i)}.c_str());
    }
    #ifndef NDEBUG
        const auto msg = std::string{"Created " + std::to_string(image_views.size()) + " image views"};
        Logger::info(msg.c_str());
    #endif
}

void Swapchain::initialize_framebuffers(const VkRenderPass renderpass) noexcept
{
    if constexpr (Global::IS_DEBUG_BUILD) {
        static bool initialized = false;
        if (initialized)
            Logger::fatal_error("Swapchain framebuffers already initialized");
        initialized = true;
    }

    framebuffers.resize(image_views.size());
    for (usize i = 0; i < image_views.size(); ++i) {

        VkFramebufferCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = renderpass;
        info.attachmentCount = 1;
        info.pAttachments = &image_views[i];
        info.width = extent.width;
        info.height = extent.height;
        info.layers = 1;

        if (vkCreateFramebuffer(device, &info, nullptr, &framebuffers[i]) != VK_SUCCESS)
            Logger::fatal_error(std::string{"Failed to create framebuffer at index " + std::to_string(i)}.c_str());
    }
    #ifndef NDEBUG
        const auto msg = std::string{"Created " + std::to_string(framebuffers.size()) + " framebuffers"};
        Logger::info(msg.c_str());
    #endif
}


Swapchain::~Swapchain() noexcept
{
    if (image_available_semaphore != VK_NULL_HANDLE) {
        if constexpr (Global::IS_DEBUG_BUILD) {
            Logger::info("De-allocating swapchain semaphore");
        }
        vkDestroySemaphore(device, image_available_semaphore, nullptr);
        image_available_semaphore = VK_NULL_HANDLE;
    }

    for (auto &framebuffer : framebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }

    #ifndef NDEBUG
        if (framebuffers.size() > 0) {
            const auto msg = std::string{"De-allocated "} + std::to_string(framebuffers.size()) + " framebuffers in swapchain";
            Logger::info(msg.c_str());
        }
    #endif

    for (auto &image : image_views) {
        if (image != VK_NULL_HANDLE) {
            vkDestroyImageView(device, image, nullptr);
            image = VK_NULL_HANDLE;
        }
    }
    
    #ifndef NDEBUG
        if (image_views.size() > 0) {
            const auto msg = std::string{"De-allocated "} + std::to_string(image_views.size()) + " image views in swapchain";
            Logger::info(msg.c_str());
        }
    #endif
    
    if (swapchain != VK_NULL_HANDLE) {
        if constexpr (Global::IS_DEBUG_BUILD) {
            Logger::info("De-allocating 'VkSwapchainKHR'");
        }
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }
}
