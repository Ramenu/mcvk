#ifndef MCVK_SHADER_HPP
#define MCVK_SHADER_HPP

#include "vulkan/vulkan.h"

class Shader
{
    public:
        Shader(const char *vertex_shader_path, 
               const char *fragment_shader_path, 
               VkDevice device) noexcept;

};

#endif // MCVK_SHADER_HPP