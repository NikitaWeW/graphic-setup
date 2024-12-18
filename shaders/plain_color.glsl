#shader vertex
#version 430 core
layout(location = 0) in vec4 a_position;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projectionMat;

void main() {
    gl_Position = u_projectionMat * u_viewMat * u_modelMat * a_position;
}

#shader fragment
#version 430 core
out vec4 o_color;
uniform vec3 u_color;
void main() {
    o_color = vec4(u_color, 1);
}