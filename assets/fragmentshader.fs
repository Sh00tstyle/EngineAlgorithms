#version 330

in vec2 uv;

uniform int rowAmt;
uniform int colAmt;

uniform vec3 tileColor1;
uniform vec3 tileColor2;

uniform vec2 mousePos;

out vec4 sColor;

void main (void) {
	float x = uv.x * colAmt;	//see in which section the x of the uv is
	float xmod = floor(mod(x, 2)); //see if odd or even, floor used to obtain ints
	
	float y = uv.y * rowAmt;
	float ymod = floor(mod(y, 2));

	//the further the current pixel is away from the mousepos, the lower the intensity
	float mouseDistance = distance(mousePos, vec2(gl_FragCoord));

	float intensity = 1 - mouseDistance / 80; //mouse distance divided by spotlight size, 1 at mouse pos, decreasing the further away it is

	//if both values are the same, color the first pattern otherwise the second
	if(ymod == xmod) {
		sColor = vec4(tileColor1, 1) + vec4(1, 1, 1, 1) * intensity * 1.5f; //adding light color (white) with its intensity to the colors and adjusting its brightness
	} else {
		sColor = vec4(tileColor2, 1) + vec4(1, 1, 1, 1) * intensity * 1.5f;
	}
}

