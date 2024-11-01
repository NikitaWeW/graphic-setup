#shader vertex
#version 330 core
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in float a_texIndex;

out vec2 v_texCoord;
out float v_texIndex;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * a_position;
    v_texCoord = a_texCoord;
    v_texIndex = a_texIndex;
}

#shader fragment
#version 330 core
out vec4 color;

in vec2 v_texCoord;
flat in float v_texIndex;

uniform sampler2D u_Textures[2];

void main() {
    vec4 texColor = texture(u_Textures[int(v_texIndex)], v_texCoord);
    color = texColor;
}