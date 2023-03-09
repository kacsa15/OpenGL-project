#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;


out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
//float ambientStrength = 0.2f;
uniform float ambientStrength = 0.3f;
vec3 diffuse;
vec3 specular;
//float specularStrength = 0.5f;
float specularStrength = 0.5f;
float shininessPoint = 50.0f;

//fog 
uniform int foginit;
uniform float fogDensity = 0.003f;

//lighting for lamps
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;


float constant = 1.0f;
float linear = 0.7f;
float quadratic = 1.8f;

vec3 computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
	
	return (ambient + diffuse + specular);

}

float computeFog()
{
	//float fogDensity = 0.05f;
	vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

vec3 computePointLight(vec4 lightPosEye)
{
	vec3 cameraPosEye = vec3(0.0f);
	vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
	vec3 normalEye = normalize(normalMatrix * fNormal);
	vec3 lightDirN = normalize(lightPosEye.xyz - fPosEye.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	vec3 ambient = ambientStrength * lightColor;
	vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	vec3 halfVector = normalize(lightDirN + viewDirN);
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininessPoint);
	vec3 specular = specularStrength * specCoeff * lightColor;
	float distance = length(lightPosEye.xyz - fPosEye.xyz);
	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
	return (ambient + diffuse + specular) * att;
}

void main() 
{
    //computeDirLight();
    vec3 light = computeDirLight();
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;
	
	light += computePointLight(view * vec4(lightPos1, 1.0f)) / 2;
	//light += computePointLight(view * vec4(lightPos2, 1.0f)) / 2;
	//light += computePointLight(view * vec4(lightPos3, 1.0f)) / 2;
	
    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 8.0f);
    fColor = vec4(color, 1.0f);
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	if ( foginit == 0)
	{

		fColor = min(colorFromTexture * vec4(color, 1.0f), 1.0f);
	}
	else
	{
		//fColor = (1 - fogFactor) * colorFromTexture + fogFactor * vec4(fogColor.rgb, 1.0);
		fColor = (1 - fogFactor) * fogColor + fogFactor * (colorFromTexture * vec4(light, 1.0f));
	}
}
