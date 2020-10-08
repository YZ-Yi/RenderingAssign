//Alejandro Garcia CPSC 591/691 A#2 F2020 Boilerplate

//For more information of how the code works, look at the https://learnopengl.com/ tutorials from which this code is based off of

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include <list>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Object rotation settings
glm::mat4 rotation;
float rotSpeed = 2.5f;

//adjacent list
struct Node {
    unsigned int v;                     //vertex id(index)
    unsigned int f = 0;                  //front face bit
    unsigned int b = 0;                  //back face bit
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CPSC 591/691 A2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    
    // This shader should compute gooch shading
    Shader ourShader("../shaders/gooch.vs", "../shaders/gooch.fs");
    // This shader simply gives a colour of the outline
    Shader ourShader2("../shaders/outline.vs", "../shaders/outline.fs");


    // load model(s), default model is vase.obj, can load multiple at a time
    // -----------
    //Model ourModel("../models/car/car.obj");
    //Model ourModel("../models/engine/engine.obj");
    //Model ourModel("../models/bunny/bunny.obj");
    //Model ourModel("../models/teapot/teapot.obj");
    //Model ourModel("../models/vase/vase.obj");
    Model ourModel("../models/sphere/sphere.obj");

    unsigned int numVertices = ourModel.meshes[0].vertices.size();

    printf("Meshes: %zu\n", ourModel.meshes.size());

    //This vector will be ful of 3D points that make up all the triangles of the loaded obj
    std::vector<glm::vec3> vertices;

    //The number of triangles of a mesh is the number of vertex indices / 3

    
    std::vector< std::vector<std::list<Node>>> edgeBuffer;

    //For each mesh in the .obj (The models included typically have only 1 mesh)
    //Construct edge buffer
    for (int i = 0; i < ourModel.meshes.size(); i++)
    {
        //Print information
        printf("Mesh[%zu]\n", i);
        printf("\tvertices: %zu\n", ourModel.meshes[i].vertices.size());
        printf("\tindices: %zu\n\n", ourModel.meshes[i].indices.size());

        size_t numTriangles = ourModel.meshes[i].indices.size() / 3;
        size_t numIndices = ourModel.meshes[i].indices.size();

        unsigned int i0, i1, i2;

        std::vector<std::list<Node>> buffer;
        for (int j = 0; j < numIndices; j++) {
            list<Node> l;
            buffer.push_back(l);
        }


        //For each triangle
        for (int j = 0; j < numTriangles; j++)
        {
            //Get indices of this triangle
            i0 = ourModel.meshes[i].indices[j * 3 + 0];
            i1 = ourModel.meshes[i].indices[j * 3 + 1];
            i2 = ourModel.meshes[i].indices[j * 3 + 2];

            //sort indices
            if (i0 > i1) {
                unsigned int temp = i0;
                i0 = i1;
                i1 = temp;
            }
            if (i0 > i2) {
                unsigned int temp = i0;
                i0 = i2;
                i2 = temp;
            }
            if (i1 > i2) {
                unsigned int temp = i1;
                i1 = i2;
                i2 = temp;
            }

            //construct edge buffer
            auto eb = buffer[i0];
            bool isVIn = false;
            for (auto it = eb.begin(); it != eb.end(); ++it) {
                if ((*it).v == i1) {
                    isVIn == true;
                    break;
                }
            }
            if (!isVIn) {
                Node n;
                n.v = i1;
                buffer[i0].push_back(n);
            }

            isVIn = false;
            for (auto it = eb.begin(); it != eb.end(); ++it) {
                if ((*it).v == i2) {
                    isVIn == true;
                    break;
                }
            }
            if (!isVIn) {
                Node n;
                n.v = i2;
                buffer[i0].push_back(n);
            }

            eb = buffer[i1];
            isVIn = false;
            for (auto it = eb.begin(); it != eb.end(); ++it) {
                if ((*it).v == i2) {
                    isVIn == true;
                    break;
                }
            }
            if (!isVIn) {
                Node n;
                n.v = i2;
                buffer[i1].push_back(n);
            }

        }

        edgeBuffer.push_back(buffer);
    }

    //

    //Setup all the outline data to the GPU
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //Send all the data to the GPU
    //For you assignment, you need to compute and re-send the vertex data every frame (but the Setup done above only needs to be done once)
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    

    
    // Infinite render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        vertices.clear();

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Clear screen
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set the current active shader to shader #1
        ourShader.use();
        // This shader will have filled-in triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //LIGHTS
        glm::vec3 lightPositions[2] = { glm::vec3(0.f, 0.f, 2.f), glm::vec3(-2.f, -1.f, 2.f) };
        glm::vec3 lightIntensities[2] = { glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f) };
        glUniform3fv(glGetUniformLocation(ourShader.ID, "lightPositions"), 2, glm::value_ptr(lightPositions[0]));
        glUniform3fv(glGetUniformLocation(ourShader.ID, "lightIntensities"), 2, glm::value_ptr(lightIntensities[0]));


        //CAMERA
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::vec3 viewPos = camera.Position;
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("viewPos", viewPos);


        //ACTION
        glm::mat4 model = rotation;// The model transformation of the mesh (controlled through arrows)
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// The default vase is a bit too big for our scene, so scale it down
        float roughness = 0.3; // The roughness of the mesh [0,1]
        glm::vec3 objectColour = glm::vec3(0.722, 0.45, 0.2);

        ourShader.setMat4("model", model);
        ourShader.setFloat("roughness", roughness);
        ourShader.setVec3("objectColour", objectColour);
        
        ourModel.Draw(ourShader);

        // Set the current active shader to shader #1
        ourShader2.use();
        // This shader will not have filled-in triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //Send vertex shader data
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        //For each mesh in the .obj (The models included typically have only 1 mesh)
   //Construct edge buffer
        for (int i = 0; i < ourModel.meshes.size(); i++)
        {
            size_t numTriangles = ourModel.meshes[i].indices.size() / 3;

            unsigned int i0, i1, i2;
            Vertex v0, v1, v2;

            //For each triangle
            for (int j = 0; j < numTriangles; j++)
            {
                //Get indices of this triangle
                i0 = ourModel.meshes[i].indices[j * 3 + 0];
                i1 = ourModel.meshes[i].indices[j * 3 + 1];
                i2 = ourModel.meshes[i].indices[j * 3 + 2];

                //sort indices
                if (i0 > i1) {
                    unsigned int temp = i0;
                    i0 = i1;
                    i1 = temp;
                }
                if (i0 > i2) {
                    unsigned int temp = i0;
                    i0 = i2;
                    i2 = temp;
                }
                if (i1 > i2) {
                    unsigned int temp = i1;
                    i1 = i2;
                    i2 = temp;
                }


                //Get vertices of this triangle using indices
                v0 = ourModel.meshes[i].vertices[i0];
                v1 = ourModel.meshes[i].vertices[i1];
                v2 = ourModel.meshes[i].vertices[i2];

                //Get two edges of the triangle to compute triangle normal
                glm::vec3 a = v1.Position - v0.Position;
                glm::vec3 b = v2.Position - v1.Position;
                glm::vec3 triangleNormal = glm::normalize(glm::cross(a, b));

                //Compute centroid of the triangle
                glm::vec3 triangleCentroid = (v0.Position + v1.Position + v2.Position) / 3.f;

               glm::vec3 viewDirection = glm::normalize(triangleCentroid - viewPos);
                //glm::vec3 viewDirection = glm::vec3(0, 0, 1);

                //If the dotproduct between the centroid and the viewDirection is positive, this triangle is front facing
                if (glm::dot(triangleNormal, viewDirection) >= 0.0f)
                {
                    
                    //If this triangle is front facing, we add its 3 vertices to the vertices array
                    //Note that for your assignment, you need to reset the vertices array each frame, and compute all of this inside the infinite loop below
                    
                    vertices.push_back(v0.Position);
                    vertices.push_back(v1.Position);
                    vertices.push_back(v2.Position);
                   
                    for (auto it = (edgeBuffer[i])[i0].begin(); it != (edgeBuffer[i])[i0].end(); ++it) {
                        if ((*it).v == i1) {
                            (*it).f = (*it).f ^ 1;
                            break;
                        }
                    }
                   
                    for (auto it = edgeBuffer[i][i0].begin(); it != edgeBuffer[i][i0].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).f = (*it).f ^ 1;
                            break;
                        }
                    }
                        
                    for (auto it = edgeBuffer[i][i1].begin(); it != edgeBuffer[i][i1].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).f = (*it).f ^ 1;
                            break;
                        }
                    }
                    
                }
                
                //else, this triangle is back facing
                else {
                    for (auto it = edgeBuffer[i][i0].begin(); it != edgeBuffer[i][i0].end(); ++it) {
                        if ((*it).v == i1) {
                            (*it).b = (*it).b ^ 1;
                            break;
                        }
                    }
                        
                    for (auto it = edgeBuffer[i][i0].begin(); it != edgeBuffer[i][i0].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).b = (*it).b ^ 1;
                            break;
                        }
                    }
                        
                    for (auto it = edgeBuffer[i][i1].begin(); it != edgeBuffer[i][i1].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).b = (*it).b ^ 1;
                            break;
                        }

                    }
                        
                }
                
            }
/*
            for (int j = 0; j < 10; ++j) {
                for (auto& it : edgeBuffer[i][j])
                    cout << j << " " << it.v << " " << it.f << " " << it.b << endl;
            }
            */
        }

        //if the line is silhouette, add it to the vertices
        for (int i = 0; i < ourModel.meshes.size(); ++i) {
            Vertex v0, v1;

            for (int j = 0; j < edgeBuffer[i].size(); ++j) {
                for (auto it = edgeBuffer[i][j].begin(); it != edgeBuffer[i][j].end(); ++it) {
                    //if front bit and back bit are both 1, which means it is a silhooute
                    if ((*it).b && (*it).f) {
                        v0 = ourModel.meshes[i].vertices[i];
                        v1 = ourModel.meshes[i].vertices[(*it).v];
                        vertices.push_back(v0.Position);
                        vertices.push_back(v1.Position);
                    }
                }
            }
        }
        
        cout << vertices.size() << endl;
        //Send all the data to the GPU
        //For you assignment, you need to compute and re-send the vertex data every frame (but the Setup done above only needs to be done once)
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(0);


        //draw triangles with line thickness 3.0
        glBindVertexArray(VAO);
        glLineWidth(3.0);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void ProcessKeyboard(Camera_Movement dir, float deltaTime)
{
    float rotVelocity = rotSpeed * deltaTime;
    if (dir == FORWARD)
    {
        rotation = glm::rotate(rotation, -rotVelocity, glm::vec3(1.0, 0.0, 0.0));
    }
    if (dir == BACKWARD)
    {
        rotation = glm::rotate(rotation, rotVelocity, glm::vec3(1.0, 0.0, 0.0));
    }
    if (dir == LEFT)
    {
        rotation = glm::rotate(rotation, -rotVelocity, glm::vec3(0.0, 1.0, 0.0));
    }
    if (dir == RIGHT)
    {
        rotation = glm::rotate(rotation, rotVelocity, glm::vec3(0.0, 1.0, 0.0));
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Camera controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    //Model controls
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}