#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec3 cameraPos = mat3(view) *aPos;
    vec4 pos = projection * vec4(cameraPos, 1.0);
    gl_Position = pos.xyww;
}  