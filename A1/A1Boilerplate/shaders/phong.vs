#version 330 core

//Inputs (set through VAO's and VBO's in C++)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//Outputs (sent to fragment shader)
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

//Constants (set directly in C++)
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
	FragPos = vec3(model * vec4(aPos, 1.0));
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	Normal = normalize(normalMatrix * aNormal);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}