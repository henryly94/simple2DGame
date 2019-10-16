#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 fontColor;

void main()
{
    FragColor = vec4(fontColor, texture(texture1, TexCoords).r);
}
