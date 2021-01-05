

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition * pc.uScale + pc.uTranslate, 0.0, 1.0);
    fragColor = inColor;
}

