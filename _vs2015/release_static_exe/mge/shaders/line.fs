//LINE FRAGMENT SHADER

#version 330 // for glsl version (12 is for older versions , say opengl 2.1

out vec4 fragment_color;

uniform vec4 lineColor;

void main( void ) {
	fragment_color = lineColor;
}
