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

#ifndef MCVK_WINDOW_HPP
#define MCVK_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>          // for GLFWwindow
#include <vulkan/vulkan_core.h>  // for VkInstance, VkSurfaceKHR
#include "global.hpp"            // for DELETE_NON_COPYABLE_NON_MOVABLE_DEFAULT

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
