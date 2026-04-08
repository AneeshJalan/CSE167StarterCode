#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

struct Light {
	vec3 position;
	vec3 intensities;
	float attenuation;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};


in float sampleExtraOutput;
in vec3 fragNormal;
in vec3 vertPos;

uniform mat4 model;
uniform vec3 color;
uniform int useColor;
uniform int useNormCol;
uniform vec3 lightColor;

uniform vec3 cameraPos;
uniform Light light;
uniform Material material;
uniform float normFac;
uniform float matFac;

uniform samplerCube cubeMap;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix * fragNormal);

	vec3 normCol = (normal  + vec3(1.0f, 1.0f, 1.0f))/2;

	// Use the color passed in. An alpha of 1.0f means it is not transparent.

	vec3 vert2light = light.position - vertPos;
	vec3 norm_vert2light = normalize(vert2light);

	vec3 c_l = light.intensities / (light.attenuation*length(vert2light));

	// c_l = vec3(1.0f, 1.0f, 1.0f);

	// Diffuse Component
	vec3 diff = max(dot(norm_vert2light, normal),0) * material.diffuse;

	// Specular Component
	vec3 refLight = reflect(-norm_vert2light, normal);
	vec3 lineofsight = normalize(cameraPos - vertPos);
	float specComp = pow(max(dot(refLight, lineofsight), 0.01f), material.shininess * 128);
	// specComp = 0;
	vec3 spec = specComp * material.specular;

	vec3 amb = material.ambient;

	vec3 materialCol = c_l * (diff + spec + amb);

	// normFac = 0;

	if(specComp < 0) {
		materialCol = vec3(0.0f, 0.0f, 0.0f);
	}


	vec3 finalColor = (normFac * normCol) + (matFac * materialCol);

	if(normFac > 0.5) {
		finalColor = normCol;
	}
	else {
		finalColor = materialCol;
	}

    //fragColor = vec4(finalColor, sampleExtraOutput);
	//fragColor = vec4(material.diffuse + material.specular + material.ambient, 1.0f);

	//fragColor = vec4(normCol, 1.0f);


	if(useColor == 1) {
		fragColor = vec4(color, 1.0f);
	}
	else if(useNormCol == 1) {
		fragColor = vec4(normCol, 1.0f);
	}
	else {
		fragColor = texture(cubeMap, reflect(-lineofsight, normal));
	}

	// debug color
	fragColor = vec4(1.0f, 0.0f, 0.0f, 0.0f);
}