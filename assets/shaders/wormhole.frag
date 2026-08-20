#version 330 core

in float particleEnergy;
out vec4 color;

void main() {
    float distanceFromCenter = length(gl_PointCoord - vec2(0.5));
    float glow = smoothstep(0.5, 0.0, distanceFromCenter);
    color = vec4(0.25, 0.88, 1.0, glow * particleEnergy);
}
