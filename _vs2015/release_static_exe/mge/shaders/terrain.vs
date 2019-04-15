//TERRAIN VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

in vec3 vertex;
in vec3 normal;
in vec2 uv;

uniform sampler2D heightMapTexture;

uniform	mat4 projectionMatrix;
uniform	mat4 viewMatrix;
uniform	mat4 modelMatrix;

out vec2 texCoord; //make sure the texture coord is interpolate

out vec3 worldNormal;
out vec3 worldVertex;

out vec3 triplanarNormal;
out vec3 triplanarVertex;

vec3 calculateNormal(float sampleRange = 1.0f) {
    //offsets for the samples
    float offsetX = 1.0f / textureSize(heightMapTexture, 0).x * sampleRange;
    float offsetY = 1.0f / textureSize(heightMapTexture, 0).y * sampleRange;

    //applying offsets
    vec3 minX = vec3(uv.x - offsetX, 0, uv.y); 
    vec3 maxX = vec3(uv.x + offsetX, 0, uv.y);
    vec3 minY = vec3(uv.x, 0, (uv.y - offsetY));
    vec3 maxY = vec3(uv.x, 0, (uv.y + offsetY));

    //heights of the samples, will be the new y values since they represent height
    minX.y = texture(heightMapTexture, minX.xz).r;
    maxX.y = texture(heightMapTexture, maxX.xz).r;
    minY.y = texture(heightMapTexture, minY.xz).r;
    maxY.y = texture(heightMapTexture, maxY.xz).r;

    //invert uv.y to fit the positive z direction, since uvs are not alligned here
    minX.z *= -1.0f;
    maxX.z *= -1.0f;
    minY.z *= -1.0f;
    maxY.z *= -1.0f;
    
    //creating distance vectors between our points
    vec3 deltaVecX = maxX - minX;
    vec3 deltaVecY = maxY - minY;

    //cross product to get the normal
    return cross(deltaVecX, deltaVecY);
}

void main( void ){
    float heightmapSample = texture(heightMapTexture, uv).r; //getting texel r color at the uv position
    vec3 newVertex = vec3(vertex.x, heightmapSample, vertex.z); //new vertex in object space with height deformation

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(newVertex, 1.f); //applying mvp matrix to the new vertex

    texCoord = uv;

	worldNormal = vec3 (modelMatrix * vec4(calculateNormal(10.0f), 0)); //low sample range (light)
    worldVertex = vec3 (modelMatrix * vec4(newVertex, 1)); //getting vertex position in world space
    
    //object space
    triplanarNormal = calculateNormal(10.0f); //high sample range (triplanar), samples the whole texture (makes no sense?)
    triplanarVertex = newVertex; //newly calculated mesh vertex pos
}