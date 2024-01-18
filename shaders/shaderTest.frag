#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

in vec4 fragPosLightSpace;
out vec4 fColor;
in vec4 fragPos;

uniform mat4 view;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

//second light source
uniform vec3 lampLightDir;
uniform vec3 lampLightColor;

//second lamp
uniform vec3 lampLightDir2;
uniform vec3 lampLightColor2;

//third lamp
uniform vec3 lampLightDir3;
uniform vec3 lampLightColor3;

//fourth lamp
uniform vec3 lampLightDir4;
uniform vec3 lampLightColor4;

//fifth lamp
uniform vec3 lampLightDir5;
uniform vec3 lampLightColor5;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//shadow map
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.1f;
float quadratic = 0.4f;

float shadow;
uniform float on;

float computeFog() {
	float fogDensity = 0.05f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow() 
{
	float bias = 0.005f;
	//perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	
	if (normalizedCoords.z > 1.0f)
		return 0.0f;

	float currentDepth = normalizedCoords.z;

	//check if current pos in shadow
	return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
}


void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

vec3 CalcDirLight(vec4 lightDirPos, vec3 fragPos, vec3 viewDir) 
{
    vec3 normalEye = normalize(fNormal);
    vec3 lightDir = normalize(lightDirPos.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normalEye, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normalEye);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance  = length(lightDirPos.xyz - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = vec3(texture(diffuseTexture, fTexCoords)) * lampLightColor;
    vec3 diffuse  = diff * vec3(texture(diffuseTexture, fTexCoords)) * lampLightColor;
    vec3 specular = spec * vec3(texture(specularTexture, fTexCoords)) * lampLightColor;
    ambient  *= attenuation * 3;
    diffuse  *= attenuation * 3;
    specular *= attenuation * 3;
    return (ambient + diffuse + specular);
}

void main() 
{
	computeLightComponents();
	vec3 viewDir = normalize(-fPosEye.xyz);
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	
	if (on == 1.0f) {
		color = CalcDirLight(view * vec4(lampLightDir, 1.0f), fragPos.xyz, viewDir);
		color += CalcDirLight(view * vec4(lampLightDir2, 1.0f), fragPos.xyz, viewDir);
		color += CalcDirLight(view * vec4(lampLightDir3, 1.0f), fragPos.xyz, viewDir);
		color += CalcDirLight(view * vec4(lampLightDir4, 1.0f), fragPos.xyz, viewDir);
		color += CalcDirLight(view * vec4(lampLightDir5, 1.0f), fragPos.xyz, viewDir);
	}

	//fog computation
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	//fColor = mix(fogColor, color, fogFactor);
	fColor = (1-fogFactor) * fogColor + vec4(color, 1.0f) * fogFactor;
}
