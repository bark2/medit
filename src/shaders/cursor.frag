#version 330 core
uniform vec3 cursor_color;
out vec4 color;

void main()
{
	color = vec4(cursor_color, 1.0);
}