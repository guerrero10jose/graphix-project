#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// other headers
#include "models.h"
#include "shaders.h"
#include "camera.h"
#include "lights.h"

/* Global Variables */
float window_height = 800.f, window_width = 800.f;
float x_mod = 0, x_cam = 0, y_cam = 0;

// Camera (perspective initial)
Camera camera(window_width, window_height);

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    // when user presses D
    if (key == GLFW_KEY_D &&
        action == GLFW_REPEAT) {
        // move bunny to the right
        x_cam -= 1.0f;
    }

    if (key == GLFW_KEY_A &&
        action == GLFW_REPEAT) {
        // move bunny to the right
        x_cam += 1.0f;
    }

    if (key == GLFW_KEY_1 &&
        action == GLFW_PRESS) {
        camera.changePersp();
    }

    if (key == GLFW_KEY_W &&
        action == GLFW_REPEAT) {
        // move bunny to the right
        y_cam += 1.0f;
    }

    if (key == GLFW_KEY_S &&
        action == GLFW_REPEAT) {
        // move bunny to the right
        y_cam -= 1.0f;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Machine Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /* Initialize GLAD */
    gladLoadGL();

    // depth testing
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, window_width, window_height);
    glEnable(GL_SCISSOR_TEST);

    /* Screen Space */
    // Should be same size as window
    // glViewport(0, 0, window_width, window_height);

    /* Set Callback function */
    glfwSetKeyCallback(window, Key_Callback);

    /* Load Vertex/Fragment Shaders*/
    // For Objects
    Shader vertexShader("Shaders/sample.vert", 'v');
    Shader fragShader("Shaders/sample.frag", 'f');

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader.getShader());
    glAttachShader(shaderProg, fragShader.getShader());
    glLinkProgram(shaderProg);

    // For Skybox
    Shader skybox_VS("Shaders/skybox.vert", 'v');
    Shader skybox_FS("Shaders/skybox.frag", 'f');

    GLuint skybox_shaderProg = glCreateProgram();
    glAttachShader(skybox_shaderProg, skybox_VS.getShader());
    glAttachShader(skybox_shaderProg, skybox_FS.getShader());
    glLinkProgram(skybox_shaderProg);

    // finalize
    glLinkProgram(skybox_shaderProg);

    /*
      7--------6
     /|       /|
    4--------5 |
    | |      | |
    | 3------|-2
    |/       |/
    0--------1
    */
    //Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    std::string facesSkybox[]{
        "Skybox/ocean/right.png",
        "Skybox/ocean/left.png",
        "Skybox/ocean/top.png",
        "Skybox/ocean/bottom.png",
        "Skybox/ocean/front.png",
        "Skybox/ocean/back.png"
    };

    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCChannel;

        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );

            stbi_image_free(data);
        }
    }

    stbi_set_flip_vertically_on_load(true);

    /* Initialize Mesh Stuff*/
    // try shark
    Model shark("shark.obj", "3D/textures/shark.png");
    bool loaded = shark.loadMesh();
    shark.fillVertexData();

    GLfloat vertices[]{
        -0.5f, -0.5f, 0,
        0, 0.5f, 0,
        0.5, -0.5f, 0
    };

    GLuint indices[]{
        0, 1, 2
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * shark.getVertexData().size(),
        shark.getVertexData().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0);

    // norm ptr
    GLintptr normPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr
    );

    // uv ptr
    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Create Matrix */
    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    float x, y, z;
    x = y = z = 0.0f;
    y = -0.5f;

    glm::mat4 translation =
        glm::translate(identity_matrix4,
            glm::vec3(x, y, z));

    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 2.f;

    glm::mat4 scale =
        glm::scale(identity_matrix4,
            glm::vec3(scale_x, scale_y, scale_z));

    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0;
    rot_y = 1.0f;

    float theta = 90.0f;

    glm::mat4 rotation =
        glm::rotate(identity_matrix4,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));


    /* Lighting Variables */
    glm::vec3 lightPos = glm::vec3(-10, 3, 0);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    float ambientStr = 0.1f;
    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;
    float specPhong = 16.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        theta += 0.1f;

        /* Camera */
        // camera position
        glm::vec3 cameraPos = glm::vec3(x_cam, y_cam, 10.f);

        glm::mat4 cameraPositionMatrix =
            glm::translate(glm::mat4(1.0f),
                cameraPos * -1.0f);

        // world's up / center
        glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 Center = glm::vec3(0, 0.0f, 0);

        // forward
        glm::vec3 F = glm::vec3(Center - cameraPos);
        F = glm::normalize(F);

        // right
        glm::vec3 R = glm::cross(F, WorldUp);

        // up
        glm::vec3 U = glm::cross(R, F);

        // orientation matrix
        glm::mat4 cameraOrientation = glm::mat4(1.0f);

        cameraOrientation[0][0] = R.x;
        cameraOrientation[1][0] = R.y;
        cameraOrientation[2][0] = R.z;

        cameraOrientation[0][1] = U.x;
        cameraOrientation[1][1] = U.y;
        cameraOrientation[2][1] = U.z;

        cameraOrientation[0][2] = -F.x;
        cameraOrientation[1][2] = -F.y;
        cameraOrientation[2][2] = -F.z;

        //glm::mat4 viewMatrix = cameraOrientation * cameraPositionMatrix;
        glm::mat4 viewMatrix = glm::lookAt(cameraPos, Center, WorldUp);

        // load skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skybox_shaderProg);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(viewMatrix));

        unsigned int sky_projLoc = glGetUniformLocation(skybox_shaderProg, "projection");
        glUniformMatrix4fv(sky_projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        unsigned int sky_viewLoc = glGetUniformLocation(skybox_shaderProg, "view");
        glUniformMatrix4fv(sky_viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(sky_view));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(shaderProg);

        glm::mat4 transformation_matrix = glm::mat4(1.0f);

        // translation
        transformation_matrix = glm::translate(transformation_matrix,
            glm::vec3(x, y, z - 5.f));

        // scale
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z));

        // rotate
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glBindTexture(GL_TEXTURE_2D, shark.getTexture());
        glUniform1i(tex0Address, 0);


        // diffuse stuff
        unsigned int lightAddress = glGetUniformLocation(shaderProg, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        unsigned int lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));

        // ambient stuff
        unsigned int ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        unsigned int ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        unsigned int cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(cameraPos));

        unsigned int specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(specStrAddress, specStr);

        unsigned int specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix));

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(transformation_matrix));

        /* Apply Shaders */
        glUseProgram(shaderProg);

        //glBindVertexArray(VAO);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, shark.getVertexData().size() / 8);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}