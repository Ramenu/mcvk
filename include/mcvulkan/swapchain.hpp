#ifndef MC_VULKAN_SWAPCHAIN_HPP
#define MC_VULKAN_SWAPCHAIN_HPP

#include "mcvulkan/device.hpp"
#include "mcvulkan/global.hpp"
#include <vector>

class SwapchainDetails
{
    private:
        VkSurfaceCapabilitiesKHR capabilities {}; // surface capabilities (e.g., min/max number of images in swapchain)
        std::vector<VkSurfaceFormatKHR> formats {}; // surface formats (e.g., pixel format, color space)
        std::vector<VkPresentModeKHR> presentation_modes {}; 
    public:
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(SwapchainDetails)
        SwapchainDetails(Device::PhysicalDeviceInfo physical_device, VkSurfaceKHR surface) noexcept;
        auto constexpr const &get_capabilities() const { return capabilities; }
        auto constexpr const &get_formats() const { return formats; }
        auto constexpr const &get_presentation_modes() const { return presentation_modes; }
        bool is_compatible_with_surface() const noexcept;
};

#endif // MC_VULKAN_SWAPCHAIN_HPP