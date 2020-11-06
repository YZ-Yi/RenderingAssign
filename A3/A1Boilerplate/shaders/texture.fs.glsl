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
uniform int shaderNum;
uniform	float z_min;


// texture samplers
uniform sampler2D texture1;

void main()
{   
    vec2 TexCoords;
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPositions - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 viewDis = viewPos - FragPos;

    float s =  max(dot(norm, lightDir), 0.00);
    float t;

    if(shaderNum == 1){
        t = abs(dot(norm, -viewDir));
        t = pow(t, rVal);
    }

    if(shaderNum == 2){
        t = max(dot(reflectDir, viewDir), 0.0);
        t = pow(t, rVal);
    }

    if(shaderNum == 3){
        float z_max = rVal * z_min;
        float zVal = viewDis.z;
        t = 1 - log(zVal / z_min) / log(z_max / z_min);
    }
    
    TexCoords = vec2(s, t); 
        
    FragColour = texture(texture1, TexCoords);

}