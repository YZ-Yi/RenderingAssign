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

glm::vec3 lightPositions = glm::vec3(0.f, 0.f, 10.f);
int activeTex = 0;
float rVal = 1.f;
float z_min = 0.002f;
int activeModel = 1;
int activeShader = 1;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CPSC 591/691 A3", NULL, NULL);
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


    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("../shaders/texture.vs.glsl", "../shaders/texture.fs.glsl");
    Shader ourShader1("../shaders/texture.vs.glsl", "../shaders/texture.fs.glsl");
    Shader ourShader2("../shaders/depth.vs.glsl", "../shaders/depth.fs.glsl");
    Shader ourShader3("../shaders/pm.vs.glsl", "../shaders/pm.fs.glsl");

    // load model(s), default model is vase.obj, can load multiple at a time
    // -----------
    Model sphereModel("../models/sphere.obj");
    Model ourModel("../models/head.obj");
    Model ourModel1("../models/head.obj");
    Model ourModel2("../models/boss.obj");
    Model ourModel3("../models/bunny.obj");
    Model ourModel4("../models/terrain-1.obj");
    Model ourModel5("../models/terrain-2.obj");
    Model ourModel6("../models/terrain-3.obj");

    std::string texPaths[] = { "../textures/material1.png", "../textures/material2.png", "../textures/aerial1.png", "../textures/aerial2.png" };
    unsigned int textures[4];

    //The following block-of-code was adapted from code
    //I found at the following URL:
    //https://learnopengl.com/Getting-started/Textures
    //load and create texture
    //-------------------------------------------------------------------------------
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).

    for (int i = 0; i < 4; ++i) {
        // ---------
        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load(texPaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture " << i << std::endl;
        }
        stbi_image_free(data);

    }

    //-----------------------end of attributed code-----------------------------------

    //Setup all the outline data to the GPU
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //enable this to draw in wireframe
    // -----------
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

        //load model
        switch (activeModel)
        {
        case 1:
            ourModel = ourModel1;
            break;
        case 2:
            ourModel = ourModel2;
            break;
        case 3:
            ourModel = ourModel3;
            break;
        case 4:
            ourModel = ourModel4;
            break;
        case 5:
            ourModel = ourModel5;
            break;
        case 6:
            ourModel = ourModel6;
            break;
        }


        // Clear screen
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[activeTex]);


        // don't forget to enable shader before setting uniforms
        ourShader.use();
      
        glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //LIGHTS
        glm::vec3 lightIntensities = glm::vec3(1.f, 1.f, 1.f);
        ourShader.setVec3("lightPositions", lightPositions);
        ourShader.setVec3("lightIntensities", lightIntensities);

        //CAMERA
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::vec3 viewPos = camera.Position;
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("viewPos", viewPos);
        ourShader.setInt("shaderNum", activeShader);


        //ACTION
        glm::mat4 model = rotation;// The model transformation of the mesh (controlled through arrows)
        if(activeModel == 1)
            model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// head
        else if(activeModel == 3)
            model = glm::scale(model, glm::vec3(10.f, 10.f, 10.f));	// bunny
        else if(activeModel >= 4)
            model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	// terrain
        else
            model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	
        float roughness = 0.3; // The roughness of the mesh [0,1]
        glm::vec3 objectColour = glm::vec3(0.722, 0.45, 0.2);

        ourShader.setMat4("model", model);
        ourShader.setFloat("roughness", roughness);
        ourShader.setVec3("objectColour", objectColour);
        ourShader.setFloat("rVal", rVal);
        ourShader.setFloat("z_min", z_min);
        
        ourModel.Draw(ourShader);

        //show lights position
        glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPositions);
        lightModel = glm::scale(lightModel, glm::vec3(0.1, 0.1, 0.1));
        ourShader.setMat4("model", lightModel);
        sphereModel.Draw(ourShader);
        
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

    //Select textures
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        activeTex = 0;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        activeTex = 1;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        activeTex = 2;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        activeTex = 3;

    //Select model
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        activeModel = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        activeModel = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        activeModel = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        activeModel = 4;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        activeModel = 5;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        activeModel = 6;

    //Select shader
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        activeShader = 1;
        rVal = 1.f;
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        activeShader = 2;
        rVal = 1.f;
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        activeShader = 3;
        rVal = 29.f;
    }

    //Light position controls
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        lightPositions += glm::vec3(0.f, 0.1f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        lightPositions += glm::vec3(0.f, -0.1f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        lightPositions += glm::vec3(-0.1f, 0.f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        lightPositions += glm::vec3(0.1f, 0.f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        lightPositions += glm::vec3(0.f, 0.f, 0.1f);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        lightPositions += glm::vec3(0.f, 0.f, -0.1f);

    //Parameters
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        std::cout << "Enter r: ";
        std::cin >> rVal;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        std::cout << "Enter z_min: ";
        std::cin >> z_min;
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