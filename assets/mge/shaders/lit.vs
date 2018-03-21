//LIGHTING VERTEX SHADER

#version 330 // for glsl version (12 is for older versions , say opengl 2.1

in vec3 vertex;
in vec3 normal;
in vec2 uv;

uniform	mat4 projectionMatrix;
uniform	mat4 viewMatrix;
uniform	mat4 modelMatrix;

out vec3 worldNormal;
out vec3 worldVertex;

void main( void ){
    	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.f);

		//0 for direction, 1 for position
		worldNormal = vec3 (modelMatrix * vec4 (normal, 0)); 
		worldVertex = vec3 (modelMatrix * vec4(vertex, 1)); //getting vertex position in world space
}
