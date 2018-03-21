//WOBBLING VERTEX SHADER

#version 330 // for glsl version (12 is for older versions , say opengl 2.1

in vec3 vertex;
in vec3 normal;
in vec2 uv;

uniform float time; //using time to animate stuff inside the shader

uniform	mat4 	projectionMatrix;
uniform	mat4 	viewMatrix;
uniform	mat4 	modelMatrix;

out vec2 fColor;

void main( void ){
    	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.f);

		fColor = uv; //using some interpolated color

		//Assignment 2.4, wobbling vertices
		gl_Position.x += sin(time / 500 + gl_Position.y * 5) * 0.3; //sin(Time * Speed + vertexpos * amount) * distance
}
