#shader vertex
#version 430 core
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_normal;
layout(location = 2) in vec2 a_texCoord;

out vec2 v_texCoord;
out vec3 fragPosition;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_normalMat;

void main() {
    gl_Position = u_projection * u_view * u_model * a_position;
    v_texCoord = a_texCoord;
    fragPosition = (a_position * u_model).xyz;
    v_normal = (u_normalMat * a_normal).xyz;
}

#shader fragment
#version 430 core
out vec4 color;

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 fragPosition;

uniform sampler2D texture_diffuse0;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;

void main() {
    vec4 textureColor = texture(texture_diffuse0, v_texCoord);

    float ambientStrength = .1;
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - fragPosition);

    vec3 ambient = u_lightColor * ambientStrength;
    vec3 diffuse = u_lightColor * vec3(max(dot(norm, lightDir), 0.0));

    color = vec4(ambient + diffuse, 1.0) * textureColor;
    // color = vec4(vec3(norm), 1.0);
}