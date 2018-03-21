//WOBBLING FRAGMENT SHADER

#version 330 // for glsl version (12 is for older versions , say opengl 2.1

in vec2 fColor;

out vec4 fragment_color;

void main( void ) {
	fragment_color = vec4(fColor, 1, 1);
}
