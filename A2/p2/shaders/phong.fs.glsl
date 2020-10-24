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
uniform float b;
uniform float y;
uniform float alpha;
uniform float beta;
uniform int speFlag;

void main()
{   
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightIntensities[0];
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPositions[0] - FragPos);
    float diff = dot(norm, -lightDir);
    vec3 coolVal = vec3(0.0, 0.0, b) + alpha * objectColour;
    vec3 warmVal = vec3(y, y, 0.0) + beta * objectColour;
    float val = (1.0 + diff) / 2.0;
    vec3 diffuse = val * coolVal +  (1.0  - val) * warmVal;


    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightIntensities[0];  
    

    vec3 result = diffuse;
    FragColour = vec4(result, 1.0);
}