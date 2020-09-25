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
uniform int isD;
uniform int isBec;

//Globals
float PI = 3.141592653589793f;
float E = 2.718282f;

float beckmann(vec3 norm, vec3 halfway, float r);
float ggx(vec3 norm, vec3 halfway, float r);

void main()
{   
	//Your code goes here!
	//Use functions to be organized! A function for D, G, and F are good starting points!
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(lightPositions[0] - FragPos);
    vec3 lightDir2 = normalize(lightPositions[1] - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfway1 = normalize(lightDir1 + viewDir);
    vec3 halfway2 = normalize(lightDir2 + viewDir);
    
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
    float distribution1, distribution2;
    if(isBec == 1){
        //Beckmann
        distribution1 = beckmann(norm, halfway1, roughness);
        distribution2 = beckmann(norm, halfway2, roughness);
    }
    else{
        //GGX/TR
        distribution1 = beckmann(norm, halfway1, roughness);
        distribution2 = beckmann(norm, halfway2, roughness);
    }
    
    //Sepcular BRDF
    float specBRDF1 = pow(distribution1, isD) / (4 * dot(lightDir1, norm) * dot(viewDir, norm));
    float specBRDF2 = pow(distribution2, isD) / (4 * dot(lightDir2, norm) * dot(viewDir, norm));

    float spec1 = specBRDF1 * max(dot(norm, lightDir1), 0.0) * specularStrength;
    float spec2 = specBRDF2 * max(dot(norm, lightDir2), 0.0) * specularStrength;
    vec3 specular = spec1 * lightIntensities[0] + spec2 * lightIntensities[1];

    /*
    vec3 reflectDir = reflect(-lightDir1, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightIntensities[0];  
    */
    
    vec3 result = (ambient + diffuse + specular) * objectColour;
    FragColour = vec4(result, 1.0);
}

float beckmann(vec3 norm, vec3 halfway, float r){
    float angle = acos(dot(norm, halfway));
    float cosValue = cos(angle);
    float tanValue = tan(angle);
    float numerator = pow(E, -(tanValue / r) * (tanValue / r));

    return numerator / (PI * r * r * pow(cosValue, 4));
}

float ggx(vec3 norm, vec3 halfway, float r){
    float alpha = r * r;
    float dotValue = dot(norm, halfway);
    float temp = dotValue * dotValue * (alpha * alpha - 1) + 1;

    return alpha * alpha / (PI * temp * temp);
}