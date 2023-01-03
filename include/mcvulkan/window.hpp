#ifndef MCVULKAN_WINDOW_HPP
#define MCVULKAN_WINDOW_HPP

#include <GLFW/glfw3.h>
#include "global.hpp"

class Window
{
    private:
        unsigned width {}, height {};
        GLFWwindow *window {nullptr};
    public:
        Window(unsigned width, unsigned height, const char *name) noexcept;
        Window(const Window &window) = delete;
        Window &operator=(const Window &window) = delete;
        NON_MOVABLE_DEFAULT(Window) // Delete default move assignment/constructor
        ~Window() noexcept;
        constexpr const auto get() const { return window; }
};

#endif // MCVULKAN_WINDOW_HPP
