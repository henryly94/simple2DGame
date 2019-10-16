#version 330 core
layout (location = 0) in vec4 aPos;
uniform vec2 pos;
void main()
{
    gl_Position = vec4(aPos.x + pos.x, aPos.y + pos.y, 0.0, 1.0);
}
