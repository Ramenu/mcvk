#ifndef MCVULKAN_VKCOMPONENTS_HPP 
#define MCVULKAN_VKCOMPONENTS_HPP

#include "mcvulkan/global.hpp"
#include <vulkan/vulkan.h>
#ifndef NDEBUG
    #include "mcvulkan/logger.hpp"
#endif
#include <GLFW/glfw3.h>

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
        VkComponents() noexcept;
        ~VkComponents() noexcept;
        constexpr auto get_instance() const { return instance; }
        constexpr auto get_surface() const { return surface; }
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(VkComponents)
};


#endif // MCVULKAN_VKCOMPONENTS_HPP

