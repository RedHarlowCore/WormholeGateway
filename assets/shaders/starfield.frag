#version 330 core

in float starBrightness;
out vec4 color;

void main() {
    float distanceFromCenter = length(gl_PointCoord - vec2(0.5));
    float glow = smoothstep(0.5, 0.0, distanceFromCenter);
    color = vec4(vec3(0.72, 0.86, 1.0) * starBrightness, glow);
}
