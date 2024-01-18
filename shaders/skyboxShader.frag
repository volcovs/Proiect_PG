#version 410 core

in vec3 textureCoordinates;
in vec4 fragmentPosEyeSpace;
out vec4 color;

uniform samplerCube skybox;


float computeFog() 
{
	float fogDensity = 0.05f;
	float fragmentDistance = length(fragmentPosEyeSpace);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
   //color = texture(skybox, textureCoordinates);

   float fogFactor = computeFog(); 
   vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
   vec4 auxiliary = texture(skybox, textureCoordinates);
   color = fogColor * (1 - fogFactor) + auxiliary * fogFactor;
}
