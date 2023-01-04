#include "mcvulkan/swapchain.hpp"
#include "mcvulkan/types.hpp"
#include "mcvulkan/logger.hpp"
#ifndef NDEBUG
    #include <string>
#endif

SwapchainDetails::SwapchainDetails(const Device::PhysicalDeviceInfo physical_device, VkSurfaceKHR surface) noexcept
{
    // Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device.self, surface, &capabilities);

    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.self, surface, &format_count, nullptr);

    if (format_count != 0) {
        
        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"Found "} + std::to_string(format_count) + " surface formats for device " + physical_device.name;
            Logger::diagnostic(msg.c_str());
        }

        formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.self, surface, &format_count, formats.data());
    }
    #ifndef NDEBUG
        else {
            const auto msg = std::string{"No surface formats found for device "} + physical_device.name;
            Logger::diagnostic(msg.c_str());
        }
    #endif

    u32 presentation_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.self, surface, &presentation_mode_count, nullptr);

    if (presentation_mode_count != 0) {

        if constexpr (Global::IS_DEBUG_BUILD) {
            const auto msg = std::string{"Found "} + std::to_string(presentation_mode_count) + " presentation modes for device " + physical_device.name;
            Logger::diagnostic(msg.c_str());
        }

        presentation_modes.resize(presentation_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.self, surface, &presentation_mode_count, presentation_modes.data());
    }
    #ifndef NDEBUG
        else {
            const auto msg = std::string{"No presentation modes found for device "} + physical_device.name;
            Logger::diagnostic(msg.c_str());
        }
    #endif
}

bool SwapchainDetails::is_compatible_with_surface() const noexcept
{
    return !(formats.empty() && presentation_modes.empty());
}