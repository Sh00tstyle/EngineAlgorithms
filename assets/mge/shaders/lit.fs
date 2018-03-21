//LIGHTING FRAGMENT SHADER

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

in vec3 worldNormal;
in vec3 worldVertex;

//lights
uniform int lightAmount;
uniform Light lights[32]; //array of max. 32 lights

//material properties
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

//camera position
uniform vec3 eyePosition;

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

vec3 getAmbientTerm(Light light) {
	//ambient
	return ambientColor * light.ambientContribution * diffuseColor;
}

vec3 getDiffuseTerm(Light light, vec3 lightVector, float attenuation, float spotEffect) {
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

//NOTE: main function HAS to be below all other own specified functions
void main( void ) {
	vec3 resultColor = vec3(0, 0, 0); //empty, black

	//iterating through the given amount of lights, not the whole array since it might contain unused lights
	for(int i = 0; i < lightAmount; i++) {
		Light currentLight = lights[i];

		//will change depending on the light type
		vec3 lightVector = getLightVector(currentLight);
		float attenuation = getAttenuation(currentLight, lightVector);
		float spotEffect = getSpotEffect(currentLight, lightVector);

		vec3 ambientTerm = getAmbientTerm(currentLight);
		vec3 diffuseTerm = getDiffuseTerm(currentLight, lightVector, attenuation, spotEffect);
		vec3 specularTerm = getSpecularTerm(currentLight, lightVector, attenuation, spotEffect);

		resultColor = resultColor + (ambientTerm + diffuseTerm + specularTerm); //adding new values to the old stored ones
	}

	fragment_color = vec4 (resultColor, 1); //applying all added colors to the fragment
}