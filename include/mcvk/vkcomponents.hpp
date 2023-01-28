#ifndef MCVK_VKCOMPONENTS_HPP 
#define MCVK_VKCOMPONENTS_HPP

#include <GLFW/glfw3.h>          // for GLFWwindow
#include <vulkan/vulkan_core.h>  // for VK_NULL_HANDLE, VkDebugUtilsMessenge...
#include "mcvk/global.hpp"       // for DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT

class VkComponents
{
    private:
        VkInstance instance {VK_NULL_HANDLE};
        VkSurfaceKHR surface {VK_NULL_HANDLE};
        #ifndef NDEBUG
            bool uses_debug_messenger {false}; 
            VkDebugUtilsMessengerEXT messenger {VK_NULL_HANDLE};
        #endif

    public:
        #ifndef NDEBUG
            explicit VkComponents(bool use_messenger, GLFWwindow *window) noexcept;
        #endif
        VkComponents() noexcept = default;
        ~VkComponents() noexcept;
        constexpr const auto &get_instance() const { return instance; }
        constexpr const auto &get_surface() const { return surface; }
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(VkComponents)
};


#endif // MCVK_VKCOMPONENTS_HPP

