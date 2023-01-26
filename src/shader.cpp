#include "mcvk/shader.hpp"
#include "mcvk/logger.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <array>

static VkShaderModule create_shader_module(const std::string &code, const VkDevice device) noexcept
{
    VkShaderModuleCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    info.pCode = reinterpret_cast<const u32*>(code.data());

    VkShaderModule shader_module {};
    if (vkCreateShaderModule(device, &info, nullptr, &shader_module) != VK_SUCCESS)
        Logger::fatal_error("Failed to create shader module");

    return shader_module;
}

Shader::Shader(const char *vertex_shader_path, 
               const char *fragment_shader_path,
               const VkDevice device) noexcept
{
    // open files
    std::ifstream vertex_shader_file {vertex_shader_path, std::ios::binary};
    std::ifstream fragment_shader_file {fragment_shader_path, std::ios::binary};

    if (!vertex_shader_file.is_open())
        Logger::fatal_error(std::string{std::string{"Failed to open vertex shader file: "} + vertex_shader_path}.c_str());
    if (!fragment_shader_file.is_open())
        Logger::fatal_error(std::string{std::string{"Failed to open fragment shader file: "} + fragment_shader_path}.c_str());

    // read contents from files
    std::stringstream vertex_shader_stream;
    std::stringstream fragment_shader_stream;

    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();

    vertex_shader_file.close();
    fragment_shader_file.close();

    const auto vertex_shader_code = vertex_shader_stream.str();
    const auto fragment_shader_code = fragment_shader_stream.str();

    // create shader modules and setup stages
    const auto vertex_shader_module = create_shader_module(vertex_shader_code, device);
    const auto fragment_shader_module = create_shader_module(fragment_shader_code, device);

    VkPipelineShaderStageCreateInfo vertex_shader_stage_info {}, fragment_shader_state_info {};
    vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = vertex_shader_module;
    vertex_shader_stage_info.pName = "main";

    fragment_shader_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_state_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_state_info.module = fragment_shader_module;
    fragment_shader_state_info.pName = "main";

    std::array stages {vertex_shader_stage_info, fragment_shader_state_info};
    stages = stages;
    
    vkDestroyShaderModule(device, vertex_shader_module, nullptr);
    vkDestroyShaderModule(device, fragment_shader_module, nullptr);
}