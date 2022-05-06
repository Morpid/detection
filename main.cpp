#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "main.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h" 
#include "Camera.h" 
#include "ModelAnim.h" 
#include "Model.h"
#include "animator.h"

#include <iostream>
#include <vector>

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float xWalk = 0.0f;
float zWalk = 3.0f;
float yWalk = 0.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    Shader shaderAnim("shaders/shaderAnim.vs", "shaders/shader.fs");
    Shader shader("shaders/shader.vs", "shaders/shader.fs");
    Shader cubeShader("shaders/cube.vs", "shaders/cube.fs");
    stbi_set_flip_vertically_on_load(true);
    ModelAnim Man("models/Man/Mr_Man_Walking.fbx", true);
    Animation ManAnimation("models/Man/Mr_Man_Walking.fbx", &Man);
    Animator animator(&ManAnimation);


    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces {
        "cubemap/right.jpg",
        "cubemap/left.jpg",
        "cubemap/top.jpg",
        "cubemap/bottom.jpg",
        "cubemap/front.jpg",
        "cubemap/back.jpg"

        //"cubemap/BLUE.jpeg",
        //"cubemap/BLUE.jpeg",
        //"cubemap/BLUE.jpeg",
        //"cubemap/BLUE.jpeg",
        //"cubemap/BLUE.jpeg",
        //"cubemap/BLUE.jpeg"
    };

    const char * Path = "container.jpeg";

    stbi_set_flip_vertically_on_load(false);
    unsigned int cubemapTexture = loadCubemap(faces);
    stbi_set_flip_vertically_on_load(true);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(Path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    cubeShader.use();
    cubeShader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    glm::vec3 posWalk = glm::vec3(xWalk, yWalk + 2.4f, zWalk + 3.0f);
    bool ismoving = true;
    animator.UpdateAnimation(0.0f);
    float Rotate = -180.0f;

    float LastWalkx;
    float LastWalky;
    float LastWalkz;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        //processInput(window);
        posWalk = glm::vec3(xWalk, yWalk + 2.4f, zWalk + 3.0f);
        camera.ProcessKeyboardFollow(FOLLOW, deltaTime, posWalk);

        LastWalkx = xWalk;
        LastWalky = yWalk + 2.4f;
        LastWalkz = zWalk;

        ismoving = false;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
            {
                LastWalkz = zWalk;
                zWalk -= 0.85f * deltaTime;
                ismoving = true;
                //Rotate = -180.0f;
                if(Rotate < -180.0f)
                    Rotate += 15.0f;
                if(Rotate > -180.0f)
                    Rotate -= 15.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
            {
                LastWalkz = zWalk;
                zWalk += 0.65f * deltaTime;
                ismoving = true;
                //Rotate = 0.0f;
                if(Rotate < 0.0f)
                    Rotate += 15.0f;
                if(Rotate > 0.0f)
                    Rotate -= 15.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
            {
                LastWalkx = xWalk;
                xWalk -= 0.65f * deltaTime;
                ismoving = true;
                //Rotate = -90.0f;
                if(Rotate < -90.0f)
                    Rotate += 15.0f;
                if(Rotate > -90.0f)
                    Rotate -= 15.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
            {
                LastWalkx = xWalk;
                xWalk += 0.65f * deltaTime;
                ismoving = true;
                //Rotate = 90.0f;
                if(Rotate < 90.0f)
                    Rotate += 15.0f;
                if(Rotate > 90.0f)
                    Rotate -= 15.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            LastWalkx = xWalk;
            LastWalkz = zWalk;
            zWalk -= 0.85f * deltaTime;
            xWalk -= 0.65f * deltaTime;
            ismoving = true;
            //Rotate = -135.0f;
            if(Rotate < -135.0f)
                Rotate += 15.0f;
            if(Rotate > -135.0f)
                Rotate -= 15.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            LastWalkx = xWalk;
            LastWalkz = zWalk;
            zWalk -= 0.85f * deltaTime;
            xWalk += 0.65f * deltaTime;
            ismoving = true;
            //Rotate = 135.0f;
            if(Rotate < 135.0f)
                Rotate += 15.0f;
            if(Rotate < 135.0f)
                Rotate += 15.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            LastWalkx = xWalk;
            LastWalkz = zWalk;
            xWalk -= 0.65f * deltaTime;
            zWalk += 0.65f * deltaTime;
            ismoving = true;
            //Rotate = -45.0f;
            if(Rotate < -45.0f)
                Rotate += 15.0f;
            if(Rotate < -45.0f)
                Rotate += 15.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            LastWalkx = xWalk;
            LastWalkz = zWalk;
            xWalk += 0.65f * deltaTime;
            zWalk += 0.65f * deltaTime;
            ismoving = true;
            //Rotate = 45.0f;
            if(Rotate < 45.0f)
                Rotate += 15.0f;
            if(Rotate < 45.0f)
                Rotate += 15.0f;
        }

        if(ismoving)
            animator.UpdateAnimation(deltaTime);


        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderAnim.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shaderAnim.setMat4("projection", projection);
        shaderAnim.setMat4("view", view);

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); i++)
            shaderAnim.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xWalk, yWalk, zWalk)); 
        model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
        model = glm::rotate(model, glm::radians(Rotate), glm::vec3(0.0f, 1.0f, 0.0f));	
        
        if(zWalk >= -0.7f && zWalk <= 0.7f && xWalk >= -0.7f && xWalk <= 0.7f)
        {
            zWalk = LastWalkz;
            xWalk = LastWalkx;
            model = glm::translate(model, glm::vec3(xWalk, yWalk, zWalk)); 
        }
        if(zWalk >= 0.3f && zWalk <= 1.7f && xWalk >= 0.3f && xWalk <= 1.7f)
        {
            zWalk = LastWalkz;
            xWalk = LastWalkx;
            model = glm::translate(model, glm::vec3(xWalk, yWalk, zWalk)); 
        }
        if(zWalk >= -0.7f && zWalk <= 0.7f && xWalk >= 0.3f && xWalk <= 1.7f)
        {
            zWalk = LastWalkz;
            xWalk = LastWalkx;
            model = glm::translate(model, glm::vec3(xWalk, yWalk, zWalk)); 
        }
        if(zWalk >= 0.3f && zWalk <= 1.7f && xWalk >= -0.7f && xWalk <= 0.7f)
        {
            zWalk = LastWalkz;
            xWalk = LastWalkx;
            model = glm::translate(model, glm::vec3(xWalk, yWalk, zWalk)); 
        }
        if(zWalk >= -0.7f && zWalk <= 0.7f && xWalk >= -1.7f && xWalk <= -0.7f)
        {
            zWalk = LastWalkz;
            xWalk = LastWalkx;
            model = glm::translate(model, glm::vec3(xWalk, yWalk, zWalk)); 
        }
        
        shaderAnim.setMat4("model", model);
        Man.Draw(shaderAnim);

        cubeShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        cubeShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        cubeShader.use();

        model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model         = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f)); 
        cubeShader.setMat4("model", model);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model         = glm::mat4(1.0f);
        model         = glm::translate(model, glm::vec3(0.0f, 1.6f, 0.0f)); 
        cubeShader.setMat4("model", model);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model         = glm::mat4(1.0f);
        model         = glm::translate(model, glm::vec3(1.0f, 0.6f, 1.0f)); 
        cubeShader.setMat4("model", model);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model         = glm::mat4(1.0f);
        model         = glm::translate(model, glm::vec3(1.0f, 0.6f, 0.0f)); 
        cubeShader.setMat4("model", model);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model         = glm::mat4(1.0f);
        model         = glm::translate(model, glm::vec3(0.0f, 0.6f, 1.0f)); 
        cubeShader.setMat4("model", model);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model         = glm::mat4(1.0f);
        model         = glm::translate(model, glm::vec3(-1.0f, 0.6f, 0.0f)); 
        cubeShader.setMat4("model", model);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}



unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zWalk -= 0.07f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zWalk += 0.07f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        xWalk -= 0.07f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        xWalk += 0.07f;
}
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