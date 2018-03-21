//TERRAIN FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

struct Light {
	int lightType;

	float ambientContribution;

	vec3 lightForward;
	vec3 lightPosition;

	float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

	float outerConeCos;
	float innerConeCos;

	vec3 lightColor;
};

in vec2 texCoord;
in vec3 worldNormal; //low sample range for better lighting
in vec3 worldVertex;

in vec3 triplanarNormal; //high sample range for better triplanar texturing
in vec3 triplanarVertex; //vanilla vertex without model matrix

//lights
uniform int lightAmount;
uniform Light lights[32];

//splat map textures
uniform sampler2D splatMapTexture;
uniform sampler2D diffuseTexture1;
uniform sampler2D diffuseTexture2; //water texture to animate
uniform sampler2D diffuseTexture3;
uniform sampler2D diffuseTexture4;

//material properties
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform float shininess;

//camera position
uniform vec3 eyePosition;

//needed to animate
uniform float time; 

out vec4 fragment_color;

vec3 getLightVector(Light light) {
	vec3 lightVector = light.lightForward; //Directional, only needs light direction

	if(light.lightType == 1 || light.lightType == 2) { //Point and Spot
		lightVector = worldVertex - light.lightPosition; //vectors from lightpos to vertex pos
	}

	return lightVector;
}

float getAttenuation(Light light, vec3 lightVector) {
	float attenuation = 1.0f; //Directional, no attenuation

	if(light.lightType == 1 || light.lightType == 2) { //Point and Spot
		//attenuation
		float lightDistance = length(lightVector);
		attenuation = (light.constantAttenuation + light.linearAttenuation * lightDistance + light.quadraticAttenuation * lightDistance * lightDistance);
	}

	return attenuation;
}

float getSpotEffect(Light light, vec3 lightVector) {
	float spotEffect = 1.0f; //Directional and Point, no spot effect

	if(light.lightType == 2) { //Spot
		//spotlight effect
		float directionCos = dot(normalize(lightVector), normalize(light.lightForward));
		spotEffect = smoothstep(light.outerConeCos, light.innerConeCos, directionCos); //interpolate intensity 

		//smoothstep(min, max, x):
		//returns 0 if x < min, so if we are outside of the outer cone
		//returns 1 if x >= max, so if we are inside of the inner cone
		//otherwise interpolates using Hermite interpolation between 0 and 1
	}

	return spotEffect;
}

vec3 getAmbientTerm(Light light, vec3 diffuseColor) {
	//ambient
	return ambientColor * light.ambientContribution * diffuseColor;
}

vec3 getDiffuseTerm(Light light, vec3 lightVector, float attenuation, float spotEffect, vec3 diffuseColor) {
	//diffuse
	float diffuseIntensity = max (0, dot(normalize(-lightVector), normalize (worldNormal)));
	diffuseIntensity = diffuseIntensity / attenuation * spotEffect; //applying attenuation and spot effect
	
	return diffuseIntensity * light.lightColor * diffuseColor;
}

vec3 getSpecularTerm(Light light, vec3 lightVector, float attenuation, float spotEffect) {
	//specular
	vec3 reflectedLight = reflect(lightVector, worldNormal); //reflected light ray in the normal (R)
	vec3 eyeVector = eyePosition - worldVertex; //vector from the vertex to the camera/eye position (V)
	float specularIntensity = max (0, dot(normalize(reflectedLight), normalize(eyeVector))); //max(0, R dot V)
	specularIntensity = pow (specularIntensity, shininess); //applying shininess
	specularIntensity = specularIntensity / attenuation * spotEffect; //applying attenuation and spot effect

	return specularIntensity * light.lightColor * specularColor;
}

vec4 getTriplanarDiffuse(sampler2D diffuseTexture, float scale = 1.0f) {
	//another way getting the normal in the fragment shader only using dark magic (flat shaded)
	//https://forum.unity.com/threads/getting-correct-normals-for-texturing-in-triplanar-shader.500977/
	//vec3 fragNormal = normalize(cross(dFdx(triplanarVertex), dFdy(triplanarVertex)));

	//calculate blend amount
	vec3 blending = abs(triplanarNormal); //absoulte of the normalized normal, we just care what axis it faces
	blending = normalize(max(blending, 0)); //normalize and bring to range from 0 to 1
	blending = blending / (blending.x + blending.y + blending.z); //calculating the blend factors for each axis

	//get projection from each axis
	vec4 xAxis = texture(diffuseTexture, triplanarVertex.yz * scale); //yz plane, so from "right"
	vec4 yAxis = texture(diffuseTexture, triplanarVertex.xz * scale); //xz plane, so from "top"
	vec4 zAxis = texture(diffuseTexture, triplanarVertex.xy * scale); //xy plane, so from "front"

	return xAxis * blending.x + yAxis * blending.y + zAxis * blending.z; //apply blending
}

vec3 getSplatDiffuse() {
	vec4 splatColor = texture(splatMapTexture, texCoord); //splat map color value

	/**
	//animating by shifting the uv coordinates
	vec2 waterAnimation = texCoord;
	waterAnimation.x = waterAnimation.x + sin(waterAnimation.y) * time / 8000;
	waterAnimation.y = waterAnimation.y + cos(waterAnimation.x) * time / 5000;

	//calculating that amount of each texture that will be used as a color vec
	vec4 diffuseColor1 = texture(diffuseTexture1, texCoord) * splatColor.x; //mutiplying texel at uv coords with the splatmaps channel
	vec4 diffuseColor2 = texture(diffuseTexture2, waterAnimation) * splatColor.y;
	vec4 diffuseColor3 = texture(diffuseTexture3, texCoord) * splatColor.z;
	vec4 diffuseColor4 = texture(diffuseTexture4, texCoord) * splatColor.w;
	/**/

	/**/
	//animating the water
	float waterAnimation = sin (triplanarVertex.y) * time / 8000.0f + cos (triplanarVertex.x) * time / 5000.0f;

	//triplanar texturing
	vec4 diffuseColor1 = getTriplanarDiffuse(diffuseTexture1) * splatColor.x;
	vec4 diffuseColor2 = getTriplanarDiffuse(diffuseTexture2, waterAnimation) * splatColor.y; //animated water texture
	vec4 diffuseColor3 = getTriplanarDiffuse(diffuseTexture3) * splatColor.z;
	vec4 diffuseColor4 = getTriplanarDiffuse(diffuseTexture4) * splatColor.w;
	/**/

	return (diffuseColor1 + diffuseColor2 + diffuseColor3 + diffuseColor4).xzy;
}

void main( void ) {
	vec3 diffuseColor = getSplatDiffuse();
	vec3 resultColor = vec3(0, 0, 0); //empty, black

	//iterating through the given amount of lights, not the whole array since it might contain unused lights
	for(int i = 0; i < lightAmount; i++) {
		Light currentLight = lights[i];

		//will change depending on the light type
		vec3 lightVector = getLightVector(currentLight);
		float attenuation = getAttenuation(currentLight, lightVector);
		float spotEffect = getSpotEffect(currentLight, lightVector);

		vec3 ambientTerm = getAmbientTerm(currentLight, diffuseColor);
		vec3 diffuseTerm = getDiffuseTerm(currentLight, lightVector, attenuation, spotEffect, diffuseColor);
		vec3 specularTerm = getSpecularTerm(currentLight, lightVector, attenuation, spotEffect);

		resultColor = resultColor + (ambientTerm + diffuseTerm); //no specular
	}

	fragment_color = vec4 (resultColor, 1); //applying all added colors to the fragment
}