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

#ifndef MCVK_SWAPCHAIN_HPP
#define MCVK_SWAPCHAIN_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>          // for GLFWwindow
#include <vulkan/vulkan_core.h>  // for VkImageView, VkImageView_T, VK_NULL_...
#include <cassert>               // for assert
#include <utility>               // for move
#include <vector>
#include "mcvk/global.hpp"       // for FLAG_SUM, DELETE_NON_COPYABLE_DEFAULT
#include "mcvk/types.hpp"        // for usize, u8
#include "mcvk/logger.hpp"
namespace Device { struct PhysicalDeviceInfo; }
namespace Queue { class QueueFamilyIndices; }


class Swapchain
{
    private:
        static constexpr usize __SWAPCHAIN_INDICES_CURRENT_LINE_ =  __LINE__;
        enum CompatibleFlag : u8 {
            None = 0x0,
            CompatibleWithPresentation = 0x1,
            CompatibleWithSurfaceFormat = 0x2
        } compatible_flag {};
        static constexpr usize __SWAPCHAIN_FLAGS_SUM_ = Global::FLAG_SUM(__LINE__ - __SWAPCHAIN_INDICES_CURRENT_LINE_ - 4);
        VkSwapchainKHR swapchain {VK_NULL_HANDLE};
        VkDevice device {VK_NULL_HANDLE};
        std::vector<VkImage> images {VK_NULL_HANDLE};
        std::vector<VkImageView> image_views {VK_NULL_HANDLE};
        std::vector<VkFramebuffer> framebuffers {VK_NULL_HANDLE};
        VkFormat format {};
        VkExtent2D extent {};
        VkSemaphore image_available_semaphore {VK_NULL_HANDLE};
        void initialize_image_views() noexcept;
    public:
        void initialize_framebuffers(VkRenderPass renderpass) noexcept;
        DELETE_NON_COPYABLE_DEFAULT(Swapchain)
        constexpr Swapchain(Swapchain &&other) noexcept
        {
            swapchain = other.swapchain;
            device = other.device;
            compatible_flag = other.compatible_flag;
            format = other.format;
            extent = other.extent;
            image_available_semaphore = other.image_available_semaphore;
            images = std::move(other.images);
            image_views = std::move(other.image_views);
            framebuffers = std::move(other.framebuffers);

            other.swapchain = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
            other.compatible_flag = CompatibleFlag::None;
            other.image_available_semaphore = VK_NULL_HANDLE;
            other.format = VkFormat::VK_FORMAT_UNDEFINED;
            other.extent = {0, 0};
        }
        constexpr auto &operator=(Swapchain &&other) noexcept
        {
            swapchain = other.swapchain;
            device = other.device;
            compatible_flag = other.compatible_flag;
            format = other.format;
            extent = other.extent;
            image_available_semaphore = other.image_available_semaphore;
            images = std::move(other.images);
            image_views = std::move(other.image_views);
            framebuffers = std::move(other.framebuffers);

            other.swapchain = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
            other.compatible_flag = CompatibleFlag::None;
            other.image_available_semaphore = VK_NULL_HANDLE;
            other.format = VkFormat::VK_FORMAT_UNDEFINED;
            other.extent = {0, 0};

            return *this;
        }
        constexpr Swapchain() noexcept = default;

        /**
         * NOTE: This does not initialize the framebuffers!
         */
        Swapchain(Device::PhysicalDeviceInfo physical_device, 
                  VkSurfaceKHR surface, 
                  GLFWwindow *window,
                  const Queue::QueueFamilyIndices &queue_family_indices,
                  VkDevice device) noexcept;

        constexpr bool is_compatible() const noexcept { 
            return (compatible_flag & __SWAPCHAIN_FLAGS_SUM_) == __SWAPCHAIN_FLAGS_SUM_;
        }
        constexpr const auto &framebuffer_at(usize index) const noexcept 
        { 
            if constexpr (Global::IS_DEBUG_BUILD)
                assert(index < framebuffers.size());
            return framebuffers[index]; 
        }
        constexpr const auto &image_at(usize index) const noexcept
        {
            if constexpr (Global::IS_DEBUG_BUILD)
                assert(index < images.size());
            return images[index];
        }
        constexpr const auto &get_format() const noexcept { return format; }
        constexpr const auto &get_extent() const noexcept { return extent; }
        constexpr const auto &image_available() const noexcept { return image_available_semaphore; }
        constexpr const auto &get() const noexcept { return swapchain; }
        constexpr usize size() const noexcept 
        { 
            if constexpr (Global::IS_DEBUG_BUILD)
                assert(images.size() == image_views.size());
            return images.size(); 
        }
        inline auto acquire_next_image() const noexcept
        {
            u32 image_index {};
            if (vkAcquireNextImageKHR(device, 
                                  swapchain, 
                                  Global::TIMEOUT_NS, 
                                  image_available_semaphore, 
                                  VK_NULL_HANDLE, // not using fence
                                  &image_index) != VK_SUCCESS) {
                                    Logger::fatal_error("Failed to acquire next image from swapchain");
                                  }
            return image_index;
        }
        ~Swapchain() noexcept;
};

#endif // MC_VULKAN_SWAPCHAIN_HPP
