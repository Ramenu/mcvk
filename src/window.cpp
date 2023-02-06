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

#include "mcvk/window.hpp"
#ifndef NDEBUG
    #include "mcvk/logger.hpp"
    #include "mcvk/global.hpp"
    #include <cassert>
#endif

Window::Window(int wwidth, int wheight, const char *wname) noexcept :
    width {wwidth}, height {wheight}, 
    self {[wwidth, wheight, wname](){
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        return glfwCreateWindow(wwidth, wheight, wname, nullptr, nullptr);
    }()}
{
}

Window::~Window() noexcept
{
    #ifndef NDEBUG
        Logger::info("De-allocating window");
        assert(self != nullptr);
    #endif
    glfwDestroyWindow(self);
    self = nullptr;
}

void Window::create_surface(VkInstance instance, GLFWwindow &window, VkSurfaceKHR surface) noexcept
{
    if (glfwCreateWindowSurface(instance, &window, nullptr, &surface) != VK_SUCCESS) 
        Logger::fatal_error("Failed to create window surface");
    if constexpr (Global::IS_DEBUG_BUILD)
        Logger::info("Window surface created successfully");
}
