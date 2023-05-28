//Shader=vertex

#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vc;
out vec3 Color;

void main() {
    gl_Position = vec4(position, 1);
    Color = vc;
};

//Shader=fragment

#version 330 core

out vec4 color;
in vec3 Color;

void main() {
    color = vec4(Color, 1);
};