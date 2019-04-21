#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture2;

void main()
{
    color = vec4(texture(texture_diffuse1, TexCoords));
    //color = mix(texture(texture_diffuse1, TexCoords), texture(texture2, TexCoords), 0.8);
}
