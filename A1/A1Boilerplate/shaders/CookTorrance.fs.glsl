#version 330 core
out vec4 FragColour;

//Variables, passed in from vertex shader
in vec3 FragPos;
in vec3 Normal;


//Constants, passed in directly from C++
uniform vec3 viewPos;
uniform vec3 objectColour;
uniform float roughness;
uniform vec3 lightPositions[2];
uniform vec3 lightIntensities[2];
uniform float ambientStrength;        
uniform float specularStrength;
uniform int isPi;

//Globals
float PI = 3.141592653589793f;


void main()
{   
	//Your code goes here!
	//Use functions to be organized! A function for D, G, and F are good starting points!
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(lightPositions[0] - FragPos);
    vec3 lightDir2 = normalize(lightPositions[1] - FragPos);

    
    // ambient
    vec3 ambient = ambientStrength * lightIntensities[0];
  	
    // diffuse 
    float diff1 = max(dot(norm, lightDir1), 0.0);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse = diff1 * lightIntensities[0] + diff2 * lightIntensities[1];
    if(isPi == 1)
        diffuse = diffuse / PI;

    /*
    vec3 lightDir = normalize(lightPositions[0] - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightIntensities[0];
    */

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir1, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightIntensities[0];  
        
    vec3 result = (ambient + diffuse + specular) * objectColour;
    FragColour = vec4(result, 1.0);
}
