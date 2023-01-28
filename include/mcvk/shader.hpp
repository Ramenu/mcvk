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