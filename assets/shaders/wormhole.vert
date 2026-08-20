#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float energy;
out float particleEnergy;
uniform mat4 viewProjection;

void main() {
    particleEnergy = energy;
    gl_Position = viewProjection * vec4(position, 1.0);
    gl_PointSize = 2.0 + energy * 5.0;
}
