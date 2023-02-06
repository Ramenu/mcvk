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

#ifndef MCVK_SHADER_HPP
#define MCVK_SHADER_HPP

#include "vulkan/vulkan_core.h"

class Shader
{
    public:
        Shader(const char *vertex_shader_path, 
               const char *fragment_shader_path, 
               VkDevice device) noexcept;
        Shader() noexcept = default;

};

#endif // MCVK_SHADER_HPP
