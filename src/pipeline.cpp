#include "mcvk/pipeline.hpp"
#include <array>

// these warning pushes are temporary
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

static constexpr std::array DYNAMIC_STATES {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
};

Pipeline::Pipeline(VkDevice device) noexcept :
    device_handle {device}
{
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

    // viewport and scissoring are done dynamically (at drawing time) so no need to specify them here

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


}
#pragma GCC diagnostic pop