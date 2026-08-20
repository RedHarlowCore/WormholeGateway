#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float brightness;
out float starBrightness;
uniform mat4 viewProjection;

void main() {
    starBrightness = brightness;
    gl_Position = viewProjection * vec4(position, 1.0);
    gl_PointSize = 1.0 + brightness * 3.0;
}
