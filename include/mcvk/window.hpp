#ifndef MCVK_WINDOW_HPP
#define MCVK_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "global.hpp"

class Window
{
    private:
        int width {}, height {};
    public:
        Window(int width, int height, const char *name) noexcept;
        DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT(Window)
        ~Window() noexcept;
        static void create_surface(VkInstance instance, GLFWwindow &window, VkSurfaceKHR surface) noexcept;
        GLFWwindow *self {nullptr};
        constexpr const auto &get_width() const noexcept { return this->width; }
        constexpr const auto &get_height() const noexcept { return this->height; }
};

#endif // MCVK_WINDOW_HPP
