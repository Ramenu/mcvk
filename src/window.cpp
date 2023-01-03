#include "mcvulkan/window.hpp"
#ifndef NDEBUG
    #include "mcvulkan/logger.hpp"
    #include "mcvulkan/global.hpp"
#endif

Window::Window(unsigned wwidth, unsigned wheight, const char *wname) noexcept :
    width {wwidth}, height {wheight}
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    self = glfwCreateWindow(width, height, wname, nullptr, nullptr);
}

Window::~Window() noexcept
{
    if (self != nullptr) {
        if constexpr (Global::IS_DEBUG_BUILD)
            Logger::diagnostic("De-allocating window");
        glfwDestroyWindow(self);
    }
}

void Window::create_surface(VkInstance instance, GLFWwindow &window, VkSurfaceKHR surface) noexcept
{
    if (glfwCreateWindowSurface(instance, &window, nullptr, &surface) != VK_SUCCESS) 
        Logger::fatal_error("Failed to create window surface");
    if constexpr (Global::IS_DEBUG_BUILD)
        Logger::diagnostic("Window surface created successfully");
}