#ifndef MCVULKAN_WINDOW_HPP
#define MCVULKAN_WINDOW_HPP

#include <GLFW/glfw3.h>

class Window
{
    private:
        unsigned width {}, height {};
        GLFWwindow *window {nullptr};
    public:
        Window(unsigned width, unsigned height, const char *name) noexcept;
        ~Window() noexcept;
        constexpr const auto get() const { return window; }
};

#endif // MCVULKAN_WINDOW_HPP
