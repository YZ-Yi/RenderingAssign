#version 330 core
out vec4 FragColour;

//Variables, passed in from vertex shader
in vec3 FragPos;
in vec3 Normal;


//Constants, passed in directly from C++
uniform vec3 viewPos;
uniform vec3 objectColour;
uniform vec3 materialColor;
uniform float roughness;
uniform vec3 lightPositions[2];
uniform vec3 lightIntensities[2];
uniform float ambientStrength;        
uniform float specularStrength;
uniform int isPi;
uniform int isD;
uniform int isBec;
uniform int isG;
uniform int isF;
uniform int isDeno;

//Globals
float PI = 3.141592653589793f;
float E = 2.718282f;

float beckmann(vec3 norm, vec3 halfway, float r);
float ggx(vec3 norm, vec3 halfway, float r);
float geometricFunc(vec3 inRay, vec3 outRay, vec3 norm, float r);
float g1Func(float dotVal, float r);
vec3 fresnel(vec3 outRay, vec3 halfway, vec3 f_0);

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
    vec3 diffuse = (diff1 * lightIntensities[0] + diff2 * lightIntensities[1]) * objectColour;
    if(isPi == 1)
        diffuse = diffuse / PI;


    // specular
    float distribution1, distribution2;

    //D
   //Beckmann
    distribution1 = beckmann(norm, halfway1, roughness);
    distribution2 = beckmann(norm, halfway2, roughness);
    if(isBec == 0){
        //GGX/TR
        distribution1 = beckmann(norm, halfway1, roughness);
        distribution2 = beckmann(norm, halfway2, roughness);
    }
    if(isD == 0){
        distribution1 = 1.0;
        distribution2 = 1.0;
    }

    //distribution1 = max(distribution1, 0);
    //distribution2 = max(distribution2, 0);

    //G
    float geometric1 = geometricFunc(lightDir1, viewDir, norm, roughness);
    float geometric2 = geometricFunc(lightDir2, viewDir, norm, roughness);
    if(isG == 0){
        geometric1 = 1.0;
        geometric2 = 1.0;
    }

    //F
    vec3 fre1 = fresnel(viewDir, halfway1, materialColor);
    vec3 fre2 = fresnel(viewDir, halfway2, materialColor);
    if(isF == 0){
        fre1 = vec3(1.0, 1.0, 1.0);
        fre2 = vec3(1.0, 1.0, 1.0);
    }

    //Sepcular BRDF
    float deno1 = 4.0 * dot(lightDir1, norm) * dot(viewDir, norm);
    float deno2 = 4.0 * dot(lightDir2, norm) * dot(viewDir, norm);
    if(isDeno == 0){
        deno1 = 1.0;
        deno2 = 1.0;
    }

    vec3 specBRDF1 = distribution1 * geometric1 * fre1/ deno1;
    vec3 specBRDF2 = distribution2 * geometric2 * fre2/ deno2;

    vec3 spec1 = specBRDF1 * max(dot(norm, lightDir1), 0.0) * specularStrength;
    vec3 spec2 = specBRDF2 * max(dot(norm, lightDir2), 0.0) * specularStrength;
    vec3 specular = spec1 * lightIntensities[0] + spec2 * lightIntensities[1];
    
    vec3 result = ambient + diffuse + specular;
    FragColour = vec4(result, 1.0);
}

float beckmann(vec3 norm, vec3 halfway, float r){
    float angle = acos(max(dot(norm, halfway), 0.0));
    float cosValue = cos(angle);
    float tanValue = tan(angle);
    float numerator = pow(E, -(tanValue / r) * (tanValue / r));

    return numerator / (PI * r * r * pow(cosValue, 4.0));
}

float ggx(vec3 norm, vec3 halfway, float r){
    float alpha = r * r;
    float dotValue = max(dot(norm, halfway), 0.0);
    float temp = dotValue * dotValue * (alpha * alpha - 1.0) + 1.0;

    return alpha * alpha / (PI * temp * temp);
}

float geometricFunc(vec3 inRay, vec3 outRay, vec3 norm, float r){
    float value1 = max(dot(norm, inRay), 0.0);
    float value2 = max(dot(norm, outRay), 0.0);

    return g1Func(value1, r) * g1Func(value2, r);
}

float g1Func(float value, float r){
    float k = (r + 1.0) * (r + 1.0) / 8.0;

    return value / (value * (1 - k) + k);
}

vec3 fresnel(vec3 outRay, vec3 halfway, vec3 f_0){
    float val = dot(outRay, halfway);

    return f_0 + (1.0 - f_0) * (1 - pow(val, 5.0));
}