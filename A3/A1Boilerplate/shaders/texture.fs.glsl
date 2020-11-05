#version 330 core
out vec4 FragColour;

//Input variables, passed in from vertex shader
in vec3 FragPos;
in vec3 Normal;

//Constants, passed in directly from C++
uniform vec3 viewPos;
uniform vec3 objectColour;
uniform vec3 lightPositions[2];
uniform vec3 lightIntensities[2];

// texture samplers
uniform sampler2D texture1;

void main()
{   
    vec2 TexCoords;
	float rVal = 1.0;
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPositions[0] - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    //diffuse
    //float s = max(dot(norm, lightDir), 0.00);
    float s = dot(norm, lightDir);

    float t = max(dot(norm, -viewDir), 0.00);
    //float t = dot(norm, viewDir);
    t = pow(t, rVal);
    TexCoords = vec2(s, t); 
        
    FragColour = texture(texture1, TexCoords);

}