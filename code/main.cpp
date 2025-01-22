#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <irrklang/irrKlang.h>
using namespace irrklang;

#pragma comment(lib,"assimp-vc142-mt.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"irrKlang.lib")

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float moverot = 0.0;
// camera
Camera camera(glm::vec3(0.0f, 30, 300.0));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool playmusic = false;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float transx = 0.0f;
float transz = 0.0f;
float scale = 1.0f;
int main()
{

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 将鼠标移动事件与自定义的mouse_callback函数进行关联，当鼠标在指定窗口（window）内移动时，GLFW会自动调用mouse_callback函数来处理相关逻辑
    glfwSetCursorPosCallback(window, mouse_callback);
    // 将鼠标滚轮滚动事件与自定义的scroll_callback函数进行关联，当鼠标滚轮在指定窗口（window）内滚动时，GLFW会调用scroll_callback函数来处理相应操作
    glfwSetScrollCallback(window, scroll_callback);

  
    // 告知GLFW捕获鼠标，设置输入模式，将鼠标光标模式设置为GLFW_CURSOR_DISABLED，即隐藏鼠标光标并且鼠标的移动输入会被程序完全捕获，常用于实现如第一人称视角等操作时，让鼠标操作完全服务于场景内的视角控制等功能，而不会显示常规的鼠标光标在窗口上
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

    Shader cityShader("model.vs", "model.fs");
    //Shader planeShader("plane.vs", "plane.fs");
    //GLfloat planeVertices[] = {
    //    // Positions          // Normals         // Texture Coords
    //    500, -0.5f, 500, 0.0f, 1.0f, 0.0f, 500, 0.0f,
    //    -500, -0.5f, -500, 0.0f, 1.0f, 0.0f, 0.0f, 500,
    //    -500, -0.5f, 500, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    //    500, -0.5f, 500, 0.0f, 1.0f, 0.0f, 500, 0.0f,
    //    500, -0.5f, -500, 0.0f, 1.0f, 0.0f, 500, 500,
    //    -500, -0.5f, -500, 0.0f, 1.0f, 0.0f, 0.0f, 500
    //};

    //// Setup plane VAO
    //GLuint planeVAO;
    //GLuint planeVBO;
    //glGenVertexArrays(1, &planeVAO);
    //glGenBuffers(1, &planeVBO);
    //glBindVertexArray(planeVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    //glBindVertexArray(0);
    //unsigned int diffuseMap = loadTexture("resource/ground.png");

    Model Tower("resource/Canton Tower.obj");
    Shader ourShader("model.vs", "model.fs");
    Model ourModel("resource/city-1.obj");

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(7,  20,  20),
        glm::vec3(23, 60, -40),
        glm::vec3(-40.0f,  50.0f, -120.0f),
        glm::vec3(0.0f,  100.0f, -30.0f)
    };

    Shader skyboxShader("skybox.vs", "skybox.fs");
    GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // Setup skybox VAO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    // Cubemap (Skybox)
    vector<std::string> faces
    {
       "resource/right.jpg",
       "resource/left.jpg",
       "resource/top.jpg",
       "resource/bottom.jpg",
       "resource/front.jpg",
       "resource/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    // render loop
    // -----------
    ISoundEngine* SoundEngine = createIrrKlangDevice();
    SoundEngine->play2D("resource/day.mp3", GL_TRUE);
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        cityShader.use();
        cityShader.setVec3("viewPos", camera.Position);
        cityShader.setFloat("material.shininess", 32.0f);
        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        cityShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        cityShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        cityShader.setVec3("dirLight.specular", 0.3f, 0.3f, 0.3f);
        // point light 1
        cityShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        cityShader.setVec3("pointLights[0].ambient", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[0].diffuse", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        cityShader.setFloat("pointLights[0].constant", 1.0f);
        cityShader.setFloat("pointLights[0].linear", 0.09f);
        cityShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        cityShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        cityShader.setVec3("pointLights[1].ambient", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[1].diffuse", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        cityShader.setFloat("pointLights[1].constant", 1.0f);
        cityShader.setFloat("pointLights[1].linear", 0.09f);
        cityShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        cityShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        cityShader.setVec3("pointLights[2].ambient", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[2].diffuse", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        cityShader.setFloat("pointLights[2].constant", 1.0f);
        cityShader.setFloat("pointLights[2].linear", 0.09f);
        cityShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        cityShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        cityShader.setVec3("pointLights[3].ambient", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[3].diffuse", 0.3f, 0.3f, 0.3f);
        cityShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        cityShader.setFloat("pointLights[3].constant", 1.0f);
        cityShader.setFloat("pointLights[3].linear", 0.09f);
        cityShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        cityShader.setVec3("spotLight.position", camera.Position);
        cityShader.setVec3("spotLight.direction", camera.Front);
        cityShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        cityShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        cityShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        cityShader.setFloat("spotLight.constant", 1.0f);
        cityShader.setFloat("spotLight.linear", 0.09f);
        cityShader.setFloat("spotLight.quadratic", 0.032f);
        cityShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        cityShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // configure transformation matrices

        cityShader.use();//将这个 cityShader 所代表的着色器程序设置为当前 OpenGL 环境下的活动着色器。
        ////创建透视投影矩阵主要用于模拟人眼观察物体时产生的近大远小的视觉效果
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();//创建视图矩阵
        cityShader.setMat4("projection", projection);
        cityShader.setMat4("view", view);


        // draw Tower
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(transx, 0.0f, transz));
        model = glm::rotate(model, float(glm::radians(moverot)), glm::vec3(.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));
        cityShader.setMat4("model", model);
        Tower.Draw(cityShader);



        //planeShader.use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, diffuseMap);
        //planeShader.setMat4("projection", projection);
        //planeShader.setMat4("view", view);
        //planeShader.setInt("texture_diffuse1", 0);
        //glBindVertexArray(planeVAO);
        //glm::mat4 groundmodel = glm::mat4(1.0f);
        //groundmodel = glm::translate(groundmodel, glm::vec3(0.0, 0.0, 0.0));
        ////model = glm::rotate(model, rotdegree, glm::vec3(1.0, 0.0, 0.0));
        //planeShader.setMat4("model", groundmodel);
        //glDrawArrays(GL_TRIANGLES, 0, 6);



        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // be sure to activate shader when setting uniforms/drawing objects
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.3f, 0.3f, 0.3f);
        // point light 1
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[0].diffuse", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader.setVec3("pointLights[1].ambient", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[1].diffuse", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09f);
        ourShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader.setVec3("pointLights[2].ambient", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[2].diffuse", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09f);
        ourShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader.setVec3("pointLights[3].ambient", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[3].diffuse", 0.3f, 0.3f, 0.3f);
        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09f);
        ourShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        

        //渲染街景
        model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(-500.0f, 3.0f, 0.0f));//将模型移到中间
        //model = glm::rotate(model, float(glm::radians(-90.0)), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);




        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        transx -=1.0;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        transx += 1.0;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        transz -= 1.0;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        transz += 1.0;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        scale +=0.01;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        scale -=0.01;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        moverot += 1;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        moverot -= 1;

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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

GLuint loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}