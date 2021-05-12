#version 460

in float Transp;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2D ParticleTex;


void main() {
   FragColor = texture(ParticleTex,TexCoord);

   FragColor = vec4(mix(vec3(0,0,0), FragColor.xyz, Transp), FragColor.a);

   FragColor.a *= Transp;
}
