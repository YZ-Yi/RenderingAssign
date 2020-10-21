//Alejandro Garcia CPSC 591/691 A#1 F2020 Boilerplate

//Code heavily influenced by https://learnopengl.com/ tutorials

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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera settings
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Object rotation settings
glm::mat4 rotation;
float rotSpeed = 2.5f;

int modelNum = 0;
float b = 1.f;
float y = 1.f;
float alpha = 0.5f;
float beta = 0.5f;
int pauseFlag = 0;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CPSC 591/691 A1", NULL, NULL);
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
    Shader ourShader("../shaders/Phong.vs.glsl", "../shaders/Phong.fs.glsl");
    Shader ourShader2("../shaders/outline.vs", "../shaders/outline.fs");


    // load model(s), default model is vase.obj, can load multiple at a time
    // -----------
    Model ourModel("../models/teapot/teapot.obj");
    Model ourModel1("../models/teapot/teapot.obj");
    Model ourModel2("../models/bunny/bunny.obj");


    std::cout << "Select Model:" << std::endl;
    std::cout << "1. Teapot\t 2.Bunny" << std::endl;
    std::cout << "b: enter b value[0.0, 1.0]" << std::endl;
    std::cout << "y: enter y value[0.0, 1.0]" << std::endl;
    std::cout << "z: enter alpha value[0.0, 1.0]" << std::endl;
    std::cout << "x: enter beta value[0.0, 1.0]" << std::endl;
    std::cout << "j: rendering silhouette" << std::endl;
    std::cout << "k: not rendering silhouette" << std::endl;
    

    //enable this to draw in wireframe
    // -----------
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //This vector will be ful of 3D points that make up all the triangles of the loaded obj
    std::vector<glm::vec3> vertices;

    //The number of triangles of a mesh is the number of vertex indices / 3

    std::vector<std::list<Node>> edgeBuffer;
    std::vector<std::list<Node>> edgeBuffer1;
    std::vector<std::list<Node>> edgeBuffer2;

    

    //load teapot edge buffer
    for (unsigned int j = 0; j < ourModel1.meshes[0].indices.size(); j++) {
        list<Node> l;
        edgeBuffer1.push_back(l);
    }


    for (unsigned int j = 0; j < ourModel1.meshes[0].indices.size() / 3; j++)
    {
        size_t numTriangles = ourModel1.meshes[0].indices.size() / 3;
        size_t numIndices = ourModel1.meshes[0].indices.size();
        size_t numVertices = ourModel1.meshes[0].vertices.size();

        unsigned int i0, i1, i2;
        Vertex v0, v1, v2;
        //Get indices of this triangle
        i0 = ourModel1.meshes[0].indices[j * 3 + 0];
        i1 = ourModel1.meshes[0].indices[j * 3 + 1];
        i2 = ourModel1.meshes[0].indices[j * 3 + 2];


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
        //cout << i0 << " " << i1 << " " << i2 << endl;


        v0 = ourModel1.meshes[0].vertices[i0];
        v1 = ourModel1.meshes[0].vertices[i1];
        v2 = ourModel1.meshes[0].vertices[i2];



        //construct edge buffer
        int isVIn = false;
        for (auto it = edgeBuffer1[i0].begin(); it != edgeBuffer1[i0].end(); ++it) {
            if ((*it).v == i1) {
                isVIn = true;
                break;
            }
        }
        if (isVIn == false) {
            Node n;
            n.v = i1;
            edgeBuffer1[i0].push_back(n);
        }

        isVIn = false;
        for (auto it = edgeBuffer1[i0].begin(); it != edgeBuffer1[i0].end(); ++it) {
            if ((*it).v == i2) {
                isVIn = true;
                break;
            }
        }
        if (isVIn == false) {
            Node n;
            n.v = i2;
            edgeBuffer1[i0].push_back(n);
        }

        isVIn = false;
        for (auto it = edgeBuffer1[i1].begin(); it != edgeBuffer1[i1].end(); ++it) {
            if ((*it).v == i2) {
                isVIn = true;
                break;
            }
        }
        if (isVIn == false) {
            Node n;
            n.v = i2;
            edgeBuffer1[i1].push_back(n);

        }

    }

    //load bunny edge buffer
    for (unsigned int j = 0; j < ourModel2.meshes[0].indices.size(); j++) {
        list<Node> l;
        edgeBuffer2.push_back(l);
    }


    for (unsigned int j = 0; j < ourModel2.meshes[0].indices.size() / 3; j++)
    {
        size_t numTriangles = ourModel2.meshes[0].indices.size() / 3;
        size_t numIndices = ourModel2.meshes[0].indices.size();
        size_t numVertices = ourModel2.meshes[0].vertices.size();

        unsigned int i0, i1, i2;
        Vertex v0, v1, v2;
        //Get indices of this triangle
        i0 = ourModel2.meshes[0].indices[j * 3 + 0];
        i1 = ourModel2.meshes[0].indices[j * 3 + 1];
        i2 = ourModel2.meshes[0].indices[j * 3 + 2];


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
        //cout << i0 << " " << i1 << " " << i2 << endl;


        v0 = ourModel2.meshes[0].vertices[i0];
        v1 = ourModel2.meshes[0].vertices[i1];
        v2 = ourModel2.meshes[0].vertices[i2];



        //construct edge buffer
        int isVIn = false;
        for (auto it = edgeBuffer2[i0].begin(); it != edgeBuffer2[i0].end(); ++it) {
            if ((*it).v == i1) {
                isVIn = true;
                break;
            }
        }
        if (isVIn == false) {
            Node n;
            n.v = i1;
            edgeBuffer2[i0].push_back(n);
        }

        isVIn = false;
        for (auto it = edgeBuffer2[i0].begin(); it != edgeBuffer2[i0].end(); ++it) {
            if ((*it).v == i2) {
                isVIn = true;
                break;
            }
        }
        if (isVIn == false) {
            Node n;
            n.v = i2;
            edgeBuffer2[i0].push_back(n);
        }

        isVIn = false;
        for (auto it = edgeBuffer2[i1].begin(); it != edgeBuffer2[i1].end(); ++it) {
            if ((*it).v == i2) {
                isVIn = true;
                break;
            }
        }
        if (isVIn == false) {
            Node n;
            n.v = i2;
            edgeBuffer2[i1].push_back(n);

        }

    }

    edgeBuffer = edgeBuffer1;

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Infinite render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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

        //selec model
        switch (modelNum)
        {
        case 1:
            ourModel = ourModel1;
            edgeBuffer = edgeBuffer1;
            break;
        case 2:
            ourModel = ourModel2;
            edgeBuffer = edgeBuffer2;
        }



        // don't forget to enable shader before setting uniforms
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
        model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// The default vase is a bit too big for our scene, so scale it down
        float roughness = 0.3; // The roughness of the mesh [0,1]
        glm::vec3 objectColour = glm::vec3(0.72f, 0.4f, 0.2f);

        ourShader.setMat4("model", model);
        ourShader.setFloat("roughness", roughness);
        ourShader.setVec3("objectColour", objectColour);
        ourShader.setFloat("b", b);
        ourShader.setFloat("y", y);
        ourShader.setFloat("alpha", alpha);
        ourShader.setFloat("beta", beta);
        
        ourModel.Draw(ourShader);


        
        if (!pauseFlag) {
            // Set the current active shader to shader #1
            ourShader2.use();
            // This shader will not have filled-in triangles
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            //Send vertex shader data
            ourShader2.setMat4("projection", projection);
            ourShader2.setMat4("view", view);
            ourShader2.setMat4("model", model);

            //reset edge buffer and other
            vertices.clear();
            for (int j = 0; j < ourModel.meshes[0].indices.size(); ++j) {
                for (auto it = edgeBuffer[j].begin(); it != edgeBuffer[j].end(); ++it) {
                    (*it).b = 0;
                    (*it).f = 0;
                }
            }

            //For each triangle
            size_t numTriangles = ourModel.meshes[0].indices.size() / 3;
            for (int j = 0; j < numTriangles; j++)
            {
                unsigned int i0, i1, i2;
                Vertex v0, v1, v2;
                //Get indices of this triangle
                i0 = ourModel.meshes[0].indices[j * 3 + 0];
                i1 = ourModel.meshes[0].indices[j * 3 + 1];
                i2 = ourModel.meshes[0].indices[j * 3 + 2];

                //Get vertices of this triangle using indices
                v0 = ourModel.meshes[0].vertices[i0];
                v1 = ourModel.meshes[0].vertices[i1];
                v2 = ourModel.meshes[0].vertices[i2];

                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //YOU SHOULD USE THE ORIGIONAL ORDER FROM MESHES TO CALCULATE NORMAL!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //Get two edges of the triangle to compute triangle normal
                glm::vec3 a = v1.Position - v0.Position;
                glm::vec3 b = v2.Position - v1.Position;
                glm::vec3 triangleNormal = glm::normalize(glm::cross(a, b));
                //triangleNormal = glm::mat3(rotation) * triangleNormal;
                triangleNormal = glm::mat3(model) * triangleNormal;

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
                v0 = ourModel.meshes[0].vertices[i0];
                v1 = ourModel.meshes[0].vertices[i1];
                v2 = ourModel.meshes[0].vertices[i2];


                //Compute centroid of the triangle
                glm::vec3 triangleCentroid = (v0.Position + v1.Position + v2.Position) / 3.f;
                // triangleCentroid = glm::mat3(rotation) * triangleCentroid;
                triangleCentroid = glm::mat3(model) * triangleCentroid;

                glm::vec3 viewDirection = glm::normalize(triangleCentroid - viewPos);

                //using algorithm from lec
                //If the dotproduct between the centroid and the viewDirection is , this triangle is front facing
                if (glm::dot(triangleNormal, viewDirection) <= 0.0f)
                {

                    //If this triangle is front facing, we add its 3 vertices to the vertices array
                    //Note that for your assignment, you need to reset the vertices array each frame, and compute all of this inside the infinite loop below


                    for (auto it = edgeBuffer[i0].begin(); it != edgeBuffer[i0].end(); ++it) {
                        if ((*it).v == i1) {
                            (*it).f = (*it).f ^ 1;
                            break;
                        }
                    }

                    for (auto it = edgeBuffer[i0].begin(); it != edgeBuffer[i0].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).f = (*it).f ^ 1;
                            break;
                        }
                    }

                    for (auto it = edgeBuffer[i1].begin(); it != edgeBuffer[i1].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).f = (*it).f ^ 1;
                            break;
                        }
                    }

                }
                //else, this triangle is back facing
                else {
                    for (auto it = edgeBuffer[i0].begin(); it != edgeBuffer[i0].end(); ++it) {
                        if ((*it).v == i1) {
                            (*it).b = (*it).b ^ 1;
                            break;
                        }
                    }

                    for (auto it = edgeBuffer[i0].begin(); it != edgeBuffer[i0].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).b = (*it).b ^ 1;
                            break;
                        }
                    }

                    for (auto it = edgeBuffer[i1].begin(); it != edgeBuffer[i1].end(); ++it) {
                        if ((*it).v == i2) {
                            (*it).b = (*it).b ^ 1;
                            break;
                        }

                    }

                }



                /*
                for (int j = 0; j < 10; ++j) {
                    for (auto& it : edgeBuffer[j])
                        cout << j << " " << it.v << " " << it.f << " " << it.b << endl;
                }

                */
            }

            //if the line is silhouette, add it to the vertices
            for (int j = 0; j < edgeBuffer.size(); ++j) {
                Vertex v0, v1;
                for (auto it = edgeBuffer[j].begin(); it != edgeBuffer[j].end(); ++it) {
                    //if front bit and back bit are both 1, which means it is a silhooute
                    if ((*it).b && (*it).f) {
                        v0 = ourModel.meshes[0].vertices[j];
                        v1 = ourModel.meshes[0].vertices[(*it).v];
                        //cout << j << " "  << (*it).v << " " << (*it).b << " " << (*it).f << endl;
                        vertices.push_back(v0.Position);
                        vertices.push_back(v1.Position);
                    }
                }
            }
            //std::cout << vertices.size() << std::endl;
            // std::cout << edgeBuffer.size() << std::endl;
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

            glBindVertexArray(0);


            //draw triangles with line thickness 3.0
            glBindVertexArray(VAO);
            //glPointSize(10);
            glLineWidth(3.0);
            //glDrawArrays(GL_POINTS, 0, vertices.size());
            glDrawArrays(GL_LINES, 0, vertices.size());
            glBindVertexArray(0);
        }

        


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
    
    //Select models;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        modelNum = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        modelNum = 2;

    //b and y stuff
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        std::cout << "Enter b value:";
        std::cin >> b;
        if (b > 1.0)
            b = 1.0;
        if (b < 0.0)
            b = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        std::cout << "Enter y value:";
        std::cin >> y;
        if (y > 1.0)
            y = 1.0;
        if (y < 0.0)
            y = 0.0;
    }

    //alpha and beta stuff
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        std::cout << "Enter alpha value:";
        std::cin >> alpha;
        if (alpha > 1.f)
            alpha = 1.f;
        if (alpha < 0.f)
            alpha = 0.f;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        std::cout << "Enter beta value:";
        std::cin >> beta;
        if (beta > 1.f)
            beta = 1.f;
        if (beta < 0.f)
            beta = 0.f;
    }

    //Pause updating silhouette or not
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        pauseFlag = 0;
        cout << "redering silhouette" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        pauseFlag = 1;
        cout << "not rendering silhouette" << endl;
    }
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