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

