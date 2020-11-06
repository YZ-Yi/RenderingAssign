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
uniform float rVal;

// texture samplers
uniform sampler2D texture1;

void main()
{   
    vec2 TexCoords;
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPositions - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  

    float s =  max(dot(norm, lightDir), 0.0);

    //float t = abs(dot(reflectDir, viewDir));
    //float t = dot(reflectDir, viewDir);
    float t = max(dot(reflectDir, viewDir), 0.0);
    t = pow(t, rVal);
    TexCoords = vec2(s, t); 
        
    FragColour = texture(texture1, TexCoords);

}