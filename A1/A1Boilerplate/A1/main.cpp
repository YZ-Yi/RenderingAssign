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

//interface stuff
int modelNum = 6;       //showing vase by default
float value = 0;
float roughness = 0.00001f; // The roughness of the mesh [0,1]
glm::vec3 objectColour = glm::vec3(0.722, 0.451, 0.2);
glm::vec3 materialColor = glm::vec3(0.98, 0.82, 0.76);
float ambientStrength = 1.f;
float specularStrength = 0.7f;
int isPi = 1;
int isD = 1;
int isBec = 1;
int isG = 1;
int isF = 1;
int isDeno = 1;

//user interface
void menu() {
    std::cout << "Select Models: 1. Bunny  2. Car  3. Engine  4. Sphere  5. Teapot  6. Vase" << std::endl;
    std::cout << "o, p: Include Pi or not" << std::endl;
    std::cout << "u, i: Include D or not" << std::endl;
    std::cout << "k, l: Include G or not" << std::endl;
    std::cout << "h, j: Include F or not" << std::endl;
    std::cout << "t, y: Using Beckmann or GGX/TR to compute D" << std::endl;
    std::cout << "r: roughness [0, 1]" << std::endl;
    std::cout << "c: object color: [0, 1]" << std::endl;
    std::cout << "v: ka: [0, 1]" << std::endl;
    std::cout << "b: ks: [0, 1]" << std::endl;
    std::cout << "m: Select material's color" << std::endl;
    std::cout << "-, =: Include denominator of not" << std::endl;
}

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
    //Shader ourShader("../shaders/Phong.vs", "../shaders/Phong.fs");
    Shader ourShader("../shaders/CookTorrance.vs.glsl", "../shaders/CookTorrance.fs.glsl");

    // load model(s), default model is vase.obj, can load multiple at a time
    // -----------
    Model ourModel("../models/vase/vase.obj");
    Model vaseModel("../models/vase/vase.obj");
    Model bunnyModel("../models/bunny/bunny.obj");
    Model carModel("../models/car/car.obj");
    Model engineModel("../models/engine/engine.obj");
    Model sphereModel("../models/sphere/sphere.obj");
    Model teapotModel("../models/teapot/teapot.obj");

    //display user interface
    menu();

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

        // Clear screen
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //model selecting stuff
        switch (modelNum) {
            case 1:
                ourModel = bunnyModel;
                break;
            case 2:
                ourModel = carModel;
                break;
            case 3:
                ourModel = engineModel;
                break;
            case 4: 
                ourModel = sphereModel;
                break;
            case 5:
                ourModel = teapotModel;
                break;
            case 6:
                ourModel = vaseModel;
        }

        // don't forget to enable shader before setting uniforms
        ourShader.use();
      
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

        //set model at proper position for different models
        switch (modelNum) {
            case 1:
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            case 2:
                model = glm::scale(model, glm::vec3(10.f, 10.f, 10.f));
            case 3:
                model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
            case 4:
                model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
            case 5:
                model = glm::scale(model, glm::vec3(50.f, 50.f, 50.f));
            case 6:
                model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// The default vase is a bit too big for our scene, so scale it down
        }


        ourShader.setMat4("model", model);
        ourShader.setFloat("roughness", roughness);
        ourShader.setVec3("objectColour", objectColour);
        ourShader.setVec3("materialColor", materialColor);
        ourShader.setFloat("ambientStrength", ambientStrength);
        ourShader.setFloat("specularStrength", specularStrength);
        ourShader.setInt("isPi", isPi);
        ourShader.setInt("isD", isD);
        ourShader.setInt("isG", isG);
        ourShader.setInt("isF", isF);
        ourShader.setInt("isBec", isBec);
        ourShader.setInt("isDeno", isDeno);
        
        ourModel.Draw(ourShader);


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

    //Model selection
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        modelNum = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        modelNum = 2;
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        modelNum = 3;
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        modelNum = 4;
    if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        modelNum = 5;
    if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        modelNum = 6;

    //input scalar value
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        std::cout << "Enter roughness[0, 1]: ";
        std::cin >> roughness;

        //if out of range
        if (roughness > 1)
            roughness = 1.f;
        else if (roughness <= 0)
            roughness = 0.00001f;
    }

    //object color
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        float rColor, gColor, bColor;
        std::cout << "Enter value [0, 1] for each (R, G, B):";
        std::cin >> rColor >> gColor >> bColor;
        
        //if out of range
        if (rColor > 1)
            rColor = 1.f;
        else if (rColor < 0)
            rColor = 0.f;
        if (gColor > 1)
            gColor = 1.f;
        else if (gColor < 0)
            gColor = 0.f;
        if (bColor > 1)
            bColor = 1.f;
        else if (bColor < 0)
            bColor = 0.f;

        objectColour = glm::vec3(rColor, gColor, bColor);
    }

    //ambiemt strength
    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
        std::cout << "Enter ambient coefficient value [0, 1]:";
        std::cin >> ambientStrength;

        //if out of range
        if (ambientStrength > 1)
            ambientStrength = 1;
        else if (ambientStrength < 0)
            ambientStrength = 0;
    }

    //specular strength
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        std::cout << "Enter specular coefficient value [0, 1]: ";
        std::cin >> specularStrength;

        //if out of range
        if (specularStrength > 1)
            specularStrength = 1;
        else if (specularStrength < 0)
            specularStrength = 1;
    }

    //pi
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (isPi == -1)
            std::cout << "Pi is on" << std::endl;
        isPi = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (isPi == 1)
            std::cout << "Pi is off" << std::endl;
        isPi = -1;
    }
    
    //D stuff
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!isD)
            std::cout << "D is on" << std::endl;
        isD = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (isD)
            std::cout << "D is off" << std::endl;
        isD = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!isBec)
            std::cout << "Using Beckmann" << std::endl;
        isBec = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (isBec)
            std::cout << "Using GGX/TR" << std::endl;
        isBec = 0;
    }

    //G stuff
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!isG)
            std::cout << "G is on" << std::endl;
        isG = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (isG)
            std::cout << "G is off" << std::endl;
        isG = 0;
    }

    //F stuff
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        if (!isF)
            std::cout << "F is on" << std::endl;
        isF = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (isF)
            std::cout << "F is off" << std::endl;
        isF = 0;
    }

    //material's color
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        int op = 7;

        std::cout << "1. water 2. plastic/glass(low) 3. plastic high 4. glass(high)/ruby 5. diamond 6. iron 7. copper 8. gold 9. aluminium 10. silver" << std::endl;
        std::cout << "Select material's color:";
        std::cin >> op;

        switch (op)
        {
            case 1:
                materialColor = glm::vec3(0.15f, 0.15f, 0.15f);
                break;
            case 2:
                materialColor = glm::vec3(0.21f, 0.21f, 0.21f);
                break;
            case 3:
                materialColor = glm::vec3(0.24f, 0.24f, 0.24f);
                break;
            case 4:
                materialColor = glm::vec3(0.31f, 0.31f, 0.31f);
                break;
            case 5:
                materialColor = glm::vec3(0.45f, 0.45f, 0.45f);
                break;
            case 6:
                materialColor = glm::vec3(0.77f, 0.78f, 0.78f);
                break;
            case 7:
                materialColor = glm::vec3(0.98f, 0.82f, 0.76f);
                break;
            case 8:
                materialColor = glm::vec3(1.f, 0.86f, 0.57f);
                break;
            case 9:
                materialColor = glm::vec3(0.96f, 0.96f, 0.97f);
                break;
            case 10:
                materialColor = glm::vec3(0.98f, 0.97f, 0.95f);
        }
    }
    
    //denomiator stuff
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        if (!isDeno)
            std::cout << "Denominator is on" << std::endl;
        isDeno = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        if (isDeno)
            std::cout << "Demominator is off" << std::endl;
        isDeno = 0;
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