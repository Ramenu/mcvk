#include "mcvulkan/window.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifndef NDEBUG
    #include "mcvulkan/logger.hpp"
    #include "mcvulkan/global.hpp"
#endif

Window::Window(unsigned wwidth, unsigned wheight, const char *wname) noexcept :
    width {wwidth}, height {wheight}
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, wname, nullptr, nullptr);
}

Window::~Window() noexcept
{
    if (window != nullptr) {
        if constexpr (Global::IS_DEBUG_BUILD)
            Logger::info("Destroying window");
        glfwDestroyWindow(window);
    }
}