#ifndef MC_VULKAN_SWAPCHAIN_HPP
#define MC_VULKAN_SWAPCHAIN_HPP

#include "mcvulkan/physicaldeviceinfo.hpp"
#include "mcvulkan/types.hpp"
#include "mcvulkan/queue.hpp"
#include "mcvulkan/global.hpp"
#include "mcvulkan/logger.hpp"
#include <GLFW/glfw3.h>
#include <vector>

class Swapchain
{
    private:
        static constexpr usize __SWAPCHAIN_INDICES_CURRENT_LINE_ =  __LINE__;
        enum CompatibleFlag {
            None = 0x0,
            CompatibleWithPresentation = 0x1,
            CompatibleWithSurfaceFormat = 0x2
        } compatible_flag {};
        static constexpr usize __SWAPCHAIN_FLAGS_SUM_ = Global::FLAG_SUM(__LINE__ - __SWAPCHAIN_INDICES_CURRENT_LINE_ - 4);
        VkSwapchainKHR swapchain {VK_NULL_HANDLE};
        VkDevice device {VK_NULL_HANDLE};
        std::vector<VkImage> images {};
    public:
        DELETE_NON_COPYABLE_DEFAULT(Swapchain)
        constexpr Swapchain(Swapchain &&other) noexcept
        {
            swapchain = other.swapchain;
            device = other.device;
            compatible_flag = other.compatible_flag;
            other.swapchain = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
            other.compatible_flag = CompatibleFlag::None;
        }
        constexpr auto &operator=(Swapchain &&other) noexcept
        {
            swapchain = other.swapchain;
            device = other.device;
            compatible_flag = other.compatible_flag;
            other.swapchain = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
            other.compatible_flag = CompatibleFlag::None;
            return *this;
        }
        constexpr Swapchain() noexcept = default;

        Swapchain(Device::PhysicalDeviceInfo physical_device, 
                  VkSurfaceKHR surface, 
                  GLFWwindow *window,
                  const Queue::QueueFamilyIndices &queue_family_indices,
                  VkDevice device) noexcept;

        constexpr bool is_compatible() const noexcept { 
            return (compatible_flag & __SWAPCHAIN_FLAGS_SUM_) == __SWAPCHAIN_FLAGS_SUM_;
        }
        ~Swapchain() noexcept
        {
            if (swapchain != VK_NULL_HANDLE) {
                if (device != VK_NULL_HANDLE) {
                    if constexpr (Global::IS_DEBUG_BUILD)
                        Logger::info("De-allocating swapchain");
                    vkDestroySwapchainKHR(device, swapchain, nullptr);
                }
                else
                    Logger::fatal_error("Failed to de-allocate swapchain. Device is NULL.");

            }
        }
};

#endif // MC_VULKAN_SWAPCHAIN_HPP