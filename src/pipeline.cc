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

#include "mcvk/pipeline.hpp"

#include <array>            // for array
#include <fstream> // IWYU pragma: keep
#include <string> // IWYU pragma: keep
#include <sstream> // IWYU pragma: keep

#include "mcvk/global.hpp"  // for IS_DEBUG_BUILD
#include "mcvk/logger.hpp"  // for fatal_error
#include "mcvk/types.hpp"
#include "mcvk/device.hpp"


static constexpr std::array DYNAMIC_STATES {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

#ifndef NDEBUG
    static VkShaderModule create_shader_module(const std::string &, const Device::LogicalDevice &, const char *) noexcept;
#else
    static VkShaderModule create_shader_module(const std::string &, const Device::LogicalDevice &, VkDevice) noexcept;
#endif

Pipeline::Pipeline(const Device::LogicalDevice &device, const VkFormat swapchain_image_format) noexcept :
    device_handle {device.get()}
{
    if constexpr (Global::IS_DEBUG_BUILD)
        assert(device_handle != VK_NULL_HANDLE);
    VkPipelineDynamicStateCreateInfo dynamic_state_info {};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = DYNAMIC_STATES.size();
    dynamic_state_info.pDynamicStates = DYNAMIC_STATES.data();

    // describes the format of the vertex data that will be passed to the vertex shader
    VkPipelineVertexInputStateCreateInfo vertex_input_info {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;

    // describes the geometry to be drawn from the vertices and if primitive restart should be enabled
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info {};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    // viewport and scissoring are done dynamically (at drawing time) so we don't need to really
    // setup anything here, asides from just creating the struct and setting the viewport and scissor count
    VkPipelineViewportStateCreateInfo viewport_info {};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.scissorCount = 1;

    // the rasterizer takes the geometry shaped by the vertices from 
    // the vertex shader and turns it into fragments to be colored by the 
    // fragment shader
    VkPipelineRasterizationStateCreateInfo rasterizer_info {};
    rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // VK_TRUE clamps fragments that are beyond the near and far planes
    rasterizer_info.depthClampEnable = VK_FALSE; 

    // VK_TRUE disables any output to the framebuffer
    rasterizer_info.rasterizerDiscardEnable = VK_FALSE; 

    // fill polygons with fragments
    rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL; 

    rasterizer_info.lineWidth = 1.0f; 

    // cull back faces
    rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // the rasterizer can alter the depth values by adding a constant value or biasing them based on a fragment's slope
    rasterizer_info.depthBiasEnable = VK_FALSE; 

    // configure multisampling
    VkPipelineMultisampleStateCreateInfo multisampling_info {};
    multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_info.sampleShadingEnable = VK_FALSE;
    multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_info.minSampleShading = 1.0f; // optional
    multisampling_info.pSampleMask = nullptr; // optional
    multisampling_info.alphaToCoverageEnable = VK_FALSE; // optional
    multisampling_info.alphaToOneEnable = VK_FALSE; // optional

    // TODO: setup depth and stencil testing

    // enable color blending
    VkPipelineColorBlendAttachmentState color_blend_attachment {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                            VK_COLOR_COMPONENT_G_BIT | 
                                            VK_COLOR_COMPONENT_B_BIT | 
                                            VK_COLOR_COMPONENT_A_BIT;

    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // set blend constants and attach framebuffer
    VkPipelineColorBlendStateCreateInfo color_blending_info {};
    color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_info.logicOpEnable = VK_FALSE;
    color_blending_info.logicOp = VK_LOGIC_OP_COPY; // optional
    color_blending_info.attachmentCount = 1;
    color_blending_info.pAttachments = &color_blend_attachment;
    color_blending_info.blendConstants[0] = 0.0f; // optional
    color_blending_info.blendConstants[1] = 0.0f; // optional
    color_blending_info.blendConstants[2] = 0.0f; // optional
    color_blending_info.blendConstants[3] = 0.0f; // optional

    VkPipelineLayoutCreateInfo pipeline_layout_info {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0; // optional
    pipeline_layout_info.pSetLayouts = nullptr; // optional
    pipeline_layout_info.pushConstantRangeCount = 0; // optional
    pipeline_layout_info.pPushConstantRanges = nullptr; // optional

    if (vkCreatePipelineLayout(device_handle, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
        Logger::fatal_error("Failed to create pipeline layout");    // open files
    
    #ifndef NDEBUG
        const auto msg = "Successfully created pipeline layout (from device handle: " + device.device_name() + ")";
        Logger::info(msg.c_str());
    #endif

    this->init_renderpass(swapchain_image_format);

    /**
     * Move this later out of the pipeline constructor later.
     * Should belong in a shader class?
     */
    const char *vertex_shader_path = "./shaders/vertex.spv";
    const char *fragment_shader_path = "./shaders/fragment.spv";

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
    #ifndef NDEBUG
        const auto vertex_shader_module = create_shader_module(vertex_shader_code, device, vertex_shader_path);
        const auto fragment_shader_module = create_shader_module(fragment_shader_code, device, fragment_shader_path);
    #else
        const auto vertex_shader_module = create_shader_module(vertex_shader_code, device);
        const auto fragment_shader_module = create_shader_module(fragment_shader_code, device);
    #endif

    VkPipelineShaderStageCreateInfo vertex_shader_stage_info {}, fragment_shader_state_info {};
    vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = vertex_shader_module;
    vertex_shader_stage_info.pName = "main";

    fragment_shader_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_state_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_state_info.module = fragment_shader_module;
    fragment_shader_state_info.pName = "main";

    const std::array shader_stages {vertex_shader_stage_info, fragment_shader_state_info};

    // now we can finally create the pipeline
    VkGraphicsPipelineCreateInfo pipeline_info {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &rasterizer_info;
    pipeline_info.pMultisampleState = &multisampling_info;
    pipeline_info.pDepthStencilState = nullptr; // optional
    pipeline_info.pColorBlendState = &color_blending_info;
    pipeline_info.pDynamicState = &dynamic_state_info; 

    pipeline_info.layout = this->pipeline_layout;
    pipeline_info.renderPass = this->renderpass;
    pipeline_info.subpass = 0;

    // Vulkan allows you to create a new pipeline by deriving from an existing one. This allows for
    // better performance. Right now there is only a single pipeline so we don't need it. You can also
    // specify the index of the pipeline to derive from, which is what 'basePipelineIndex' is for.
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // optional
    pipeline_info.basePipelineIndex = -1; // optional

    if (vkCreateGraphicsPipelines(device_handle, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
        Logger::fatal_error("Failed to create graphics pipeline");

    vkDestroyShaderModule(device.get(), vertex_shader_module, nullptr);
    vkDestroyShaderModule(device.get(), fragment_shader_module, nullptr);

}

static VkShaderModule create_shader_module(const std::string &code, 
                                           const Device::LogicalDevice &device
                                           #ifndef NDEBUG
                                             ,const char *shader_file_path
                                           #endif
                                           ) noexcept
{
    VkShaderModuleCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    info.pCode = reinterpret_cast<const u32*>(code.data());

    VkShaderModule shader_module {};
    if (vkCreateShaderModule(device.get(), &info, nullptr, &shader_module) != VK_SUCCESS)
        Logger::fatal_error("Failed to create shader module");

    #ifndef NDEBUG
        const auto msg = std::string{"Successfully created shader module from '"} + 
                        shader_file_path + 
                        "' from device handle: '" + 
                        device.device_name() +
                        "' (code size: " + 
                        std::to_string(code.size()) + 
                        " bytes)";
        Logger::info(msg.c_str());
    #endif

    return shader_module;
}

void Pipeline::init_renderpass(const VkFormat swapchain_image_format) noexcept
{
    static constexpr unsigned COLOR_ATTACHMENT_COUNT = 1;

    if constexpr (Global::IS_DEBUG_BUILD)
        assert(device_handle != VK_NULL_HANDLE);

    VkAttachmentDescription color_attachment {};

    // format of the color attachment should match format of the swapchain images
    color_attachment.format = swapchain_image_format;

    // not doing any multisampling yet, so stick to 1 sample
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // determine what to do with the data in the attachment before rendering and after rendering

    // clear the values to constant at start (this will cause the framebuffer to be black before drawing a new frame)
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; 
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // rendered contents will be stored in memory and can be read later

    // stencil load and store operations can be ignored for now
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // set the layout of the pixels in memory.

    // 'initialLayout' specifies the layout the attachment image will have before the render pass begins.
    // Using undefined means that we don't care what previous layout the image was in
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // 'finalLayout' specifies the layout to automatically transition to when the render pass finishes.
    // We want to present the rendered image to the screen, so we transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // TODO: add multiple subpasses instead of 1?

    VkAttachmentReference color_attachment_ref {};

    // specifies which attachment to reference by its index in the attachment descriptions array
    color_attachment_ref.attachment = 0; 

    // specify the layout of the attachment during a subpass that uses this reference
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // the index of the attachment in this array is directly referenced from the fragment shader 
    // with the layout(location = 0) out vec4 outColor directive!
    subpass.colorAttachmentCount = COLOR_ATTACHMENT_COUNT;

    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo renderpass_info {};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_info.attachmentCount = COLOR_ATTACHMENT_COUNT;
    renderpass_info.pAttachments = &color_attachment;
    renderpass_info.subpassCount = 1;
    renderpass_info.pSubpasses = &subpass;

    if (vkCreateRenderPass(device_handle, &renderpass_info, nullptr, &renderpass) != VK_SUCCESS)
        Logger::fatal_error("Failed to initialize renderpass");

    #ifndef NDEBUG
        Logger::info("Successfully initialized renderpass");
    #endif
}
