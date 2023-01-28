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
            images = std::move(other.images);
            image_views = std::move(other.image_views);
            framebuffers = std::move(other.framebuffers);

            other.swapchain = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
            other.compatible_flag = CompatibleFlag::None;
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
            images = std::move(other.images);
            image_views = std::move(other.image_views);
            framebuffers = std::move(other.framebuffers);

            other.swapchain = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
            other.compatible_flag = CompatibleFlag::None;
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
        constexpr const auto &get_format() const noexcept { return format; }
        constexpr const auto &get_extent() const noexcept { return extent; }
        constexpr usize size() const noexcept 
        { 
            if constexpr (Global::IS_DEBUG_BUILD)
                assert(images.size() == image_views.size());
            return images.size(); 
        }
        ~Swapchain() noexcept;
};

#endif // MC_VULKAN_SWAPCHAIN_HPP