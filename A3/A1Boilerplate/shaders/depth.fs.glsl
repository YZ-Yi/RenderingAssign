#version 330 core
out vec4 FragColour;

//Input variables, passed in from vertex shader
in vec3 FragPos;
in vec3 Normal;

//Constants, passed in directly from C++
uniform vec3 viewPos;
uniform vec3 objectColour;
uniform vec3 lightPositions;
uniform vec3 lightIntensities;

// texture samplers
uniform sampler2D texture1;

void main()
{   
    vec2 TexCoords;
	float rVal = 1.0;
  	float z_min = 1.0;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPositions - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    //diffuse
    //float s = max(dot(norm, lightDir), 0.00);
    float s = max(dot(norm, lightDir), 0);
    float z_max = rVal * z_min;

    float t = 1 - log(s / z_min) / log(z_max / z_min);

    TexCoords = vec2(s, t); 
        
    FragColour = texture(texture1, TexCoords);

}