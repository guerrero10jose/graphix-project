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
float x_mod = 0, mov_forback = 0, mov_updown = 0, rot_leftright = 0;
//camera center for 3rd person view with mouse movement
glm::vec3 Center = glm::vec3(0, 0.0f, -1.f);

/* For Player Controls */
float theta_ship = 180.f;
float theta_mod  = 180.f;
int light_setting = 0;

//mouse state
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 90.0f;
bool firstMouse = true;
float lastX = window_width / 2.0;
float lastY = window_height / 2.0;

// Camera (perspective initial)
Camera camera(window_width, window_height, 0, -145.f, 10.f);
float cameraSpeed = 1.f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; //reversed variables since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; //can change this value to anything, but will affect the sensitivity of the program
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    //prevents the pitch from going out of bounds (prevents backflipping)
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    if (yaw > -60.0f)
        yaw = -60.0f;
    if (yaw < -120.0f)
        yaw = -120.0f;

    glm::vec3 front;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    Center = glm::normalize(front);
}

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{

    if (camera.getCurrPersp() == 0) {
        if (key == GLFW_KEY_A &&
            action == GLFW_REPEAT) {

            switch (camera.getCurrentCam()) {
            case 0:
                rot_leftright -= cameraSpeed;
                Center = camera.rotateCamera(cameraSpeed, rot_leftright);
                theta_ship += cameraSpeed;
                break;
            case 1:
                theta_ship -= cameraSpeed;
                break;
            }

        }

        if (key == GLFW_KEY_D &&
            action == GLFW_REPEAT) {

            switch (camera.getCurrentCam()) {
            case 0:
                rot_leftright += cameraSpeed;
                Center = camera.rotateCamera(cameraSpeed, rot_leftright);
                theta_ship -= cameraSpeed;
                break;
            case 1:
                theta_ship += cameraSpeed;
                break;
            }
        }

        // forward
        if (key == GLFW_KEY_W &&
            action == GLFW_REPEAT) {
            camera.updateCameraPos(cameraSpeed, 'f');
            mov_forback -= cameraSpeed;
        }

        // backward
        if (key == GLFW_KEY_S &&
            action == GLFW_REPEAT) {
            camera.updateCameraPos(cameraSpeed, 'b');
            mov_forback += cameraSpeed;
        }

        if (key == GLFW_KEY_Q &&
            action == GLFW_REPEAT) {
            camera.updateCameraPos(cameraSpeed, 'q');
            mov_updown += cameraSpeed;
        }

        if (key == GLFW_KEY_E &&
            action == GLFW_REPEAT) {
            camera.updateCameraPos(cameraSpeed, 'e');
            mov_updown -= cameraSpeed;
        }

        if (key == GLFW_KEY_1 &&
            action == GLFW_PRESS) {
            // Change to third person moveable
            camera.changeCam();
        }
    }
    else {
        if (key == GLFW_KEY_W &&
            action == GLFW_REPEAT) {
            camera.movOrtho('w', cameraSpeed);
        }

        if (key == GLFW_KEY_S &&
            action == GLFW_REPEAT) {
            camera.movOrtho('s', cameraSpeed);
        }

        if (key == GLFW_KEY_A &&
            action == GLFW_REPEAT) {
            camera.movOrtho('a', cameraSpeed);
        }

        if (key == GLFW_KEY_D &&
            action == GLFW_REPEAT) {
            camera.movOrtho('d', cameraSpeed);
        }


    }
    

    if (key == GLFW_KEY_2 &&
        action == GLFW_PRESS) {
        // Change to ortho
        camera.changePerspective();
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        light_setting++;
        light_setting = light_setting % 3;
    }
}

void EmptyCallBack(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {
    // just to switch between disabled and not
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

    /* Screen Space */
    // Should be same size as window
    glViewport(0, 0, window_width, window_height);

    /* Set Callback function */
    glfwSetKeyCallback(window, Key_Callback);

    /* Load Vertex/Fragment Shaders*/
    // For Objects
    // Third Person
    Shader vertexShader("Shaders/sample.vert", 'v');
    Shader fragShader("Shaders/sample.frag", 'f');

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader.getShader());
    glAttachShader(shaderProg, fragShader.getShader());
    glLinkProgram(shaderProg);

    // First Person
    Shader sonar_VS("Shaders/sample.vert", 'v');
    Shader sonar_FS("Shaders/sonar.frag", 'f');

    GLuint sonar_shaderProg = glCreateProgram();
    glAttachShader(sonar_shaderProg, sonar_VS.getShader());
    glAttachShader(sonar_shaderProg, sonar_FS.getShader());
    glLinkProgram(sonar_shaderProg);

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
            -800.f, -800.f, 800.f, //0
            800.f, -800.f, 800.f,  //1
            800.f, -800.f, -800.f, //2
            -800.f, -800.f, -800.f,//3
            -800.f, 800.f, 800.f,  //4
            800.f, 800.f, 800.f,   //5
            800.f, 800.f, -800.f,  //6
            -800.f, 800.f, -800.f  //7
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
    shark.setTexture();
    shark.fillVertexData();
    shark.apply();

    Model ship("Microsub.obj", "3D/textures/MicroSub_Albedo.png");
    loaded = ship.loadMesh();
    ship.setTexture2();
    ship.fillVertexData();
    ship.apply();

    Model chest("treasure_chest.obj", "3D/textures/treasure_chest.jpg");
    loaded = chest.loadMesh();
    chest.setTexture2();
    chest.fillVertexData();
    chest.apply();

    
    Model jfish("Jellyfish.obj", "3D/textures/Jellyfish.png");
    loaded = jfish.loadMesh();
    jfish.setTexture2();
    jfish.fillVertexData2();
    jfish.apply2();

    Model trident("Trident.obj", "3D/textures/Trident.jpg");
    loaded = trident.loadMesh();
    trident.setTexture2();
    trident.fillVertexData();
    trident.apply();

    Model gfish("gfish.obj", "3D/textures/gfish.png");
    loaded = gfish.loadMesh();
    gfish.setTexture();
    gfish.fillVertexData();
    gfish.apply();
    
    float x, y, z;
    x = y = z = 0.0f;
    y = -0.25f;

    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 1.f;

    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0;
    rot_y = 1.0f;

    float theta = 90.0f;

    /*
        Initialization of the Point and Direction objects, which inherits from the Light parent class.
        1. Point Light controls the light emitted from the submarine. It illuminates objects as the 
           submarine and the camera moves around
        2. Direction Light is for the light emitted top down to simulate the diffusion of light entering water.
    */
    PointLight pointLight(camera.getCameraPos(), glm::vec3(1,1,1));
    DirectionLight dirLight(glm::vec3(0,5,0), glm::vec3(0.19, 0.41, 0.35));
    glm::vec3 lightPos = camera.getCameraPos();
    glm::vec3 lightColor;
    glm::vec3 sun_lightPos = dirLight.position;
    glm::vec3 sun_lightColor = dirLight.color;
    float ambientStr;
    glm::vec3 ambientColor;
    float specStr = 0.2f;
    float specPhong = 8.0f;

    /*
    //functions gets the current window as well as the void function to get control of the mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    //enables cursor movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    */
    GLuint currShader;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Light source will follow the camera's position
        pointLight.updatePosition(camera.getCameraPos());

        lightPos = pointLight.getPosition();
        lightColor = pointLight.color_brightness;
        ambientColor = pointLight.ambientColor;
        ambientStr = pointLight.ambientStr;
        specPhong = pointLight.specular;
        specStr = pointLight.specularStr;

        // Pressing 'F' will cycle through the "flashlight" light strength
        pointLight.setBrightness(light_setting);


        //theta += 0.1f;
        //theta_ship = theta_mod;
        //theta_ship += 0.1f;

        /* Camera */
        switch (camera.getCurrentCam()) {
            case 0:
                glfwSetCursorPosCallback(window, GL_FALSE);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                currShader = shaderProg;
                break;
            case 1:
                glfwSetCursorPosCallback(window, mouse_callback);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                currShader = shaderProg;
                break;
        }

        camera.project(Center);

        // load skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skybox_shaderProg);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(camera.getViewMatrix()));

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

        glUseProgram(currShader);

        // load object model
        shark.loadModel(-35, -165.0f, -70.0f, scale_x, rot_x, rot_y, rot_z, theta);

        GLuint tex0Address = glGetUniformLocation(currShader, "tex0");
        glBindTexture(GL_TEXTURE_2D, shark.getTexture());
        glUniform1i(tex0Address, 0);
        // diffuse stuff
        unsigned int lightAddress = glGetUniformLocation(currShader, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        unsigned int lightColorAddress = glGetUniformLocation(currShader, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));
        unsigned int sun_lightAddress = glGetUniformLocation(currShader, "sun_lightPos");
        glUniform3fv(sun_lightAddress,
            1,
            glm::value_ptr(sun_lightPos));

        unsigned int sun_lightColorAddress = glGetUniformLocation(currShader, "sun_lightColor");
        glUniform3fv(sun_lightColorAddress,
            1,
            glm::value_ptr(sun_lightColor));
        // ambient stuff
        unsigned int ambientStrAddress = glGetUniformLocation(currShader, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        unsigned int ambientColorAddress = glGetUniformLocation(currShader, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        unsigned int cameraPosAddress = glGetUniformLocation(currShader, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(camera.getCameraPos()));

        unsigned int specStrAddress = glGetUniformLocation(currShader, "specStr");
        glUniform1f(specStrAddress, specStr);

        unsigned int specPhongAddress = glGetUniformLocation(currShader, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        unsigned int projLoc = glGetUniformLocation(currShader, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        unsigned int viewLoc = glGetUniformLocation(currShader, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getViewMatrix()));

        unsigned int transformLoc = glGetUniformLocation(currShader, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(shark.getTransMatrix()));

        /* Apply Shaders */
        glUseProgram(currShader);

        //glBindVertexArray(VAO);

        glBindVertexArray(shark.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, shark.getVertexData().size() / 8);

        /*
        * 
        * 
        * 
        *               2nd Obj render (ship)
        * 
        * 
        * 
        */

        glBindVertexArray(ship.getVAO());
        //ship.loadModel(0.f, -1.f + mov_updown, 10.f + mov_forback, 0.1f, rot_x, rot_y, rot_z, theta_ship);
        /*
            y - 4.f (cam depth) third person
        */
        switch (camera.getCurrentCam()) {
        case 0:
            //ship.loadModel(0.f, -1.f + mov_updown, 10.f + mov_forback, 0.1f, rot_x, rot_y, rot_z, theta_ship);
            ship.loadModel(camera.getCameraPos(), 0.1f, rot_x, rot_y, rot_z, theta_ship);
            break;
        case 1:
            ship.loadModel(camera.getCameraPos2(), 0.1f, rot_x, rot_y, rot_z, theta_ship, Center);
            break;
        }
        

        tex0Address = glGetUniformLocation(currShader, "tex0");
        glBindTexture(GL_TEXTURE_2D, ship.getTexture());
        glUniform1i(tex0Address, 0);

        // diffuse stuff
        lightAddress = glGetUniformLocation(currShader, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        lightColorAddress = glGetUniformLocation(currShader, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));
        sun_lightAddress = glGetUniformLocation(currShader, "sun_lightPos");
        glUniform3fv(sun_lightAddress,
            1,
            glm::value_ptr(sun_lightPos));

        sun_lightColorAddress = glGetUniformLocation(currShader, "sun_lightColor");
        glUniform3fv(sun_lightColorAddress,
            1,
            glm::value_ptr(sun_lightColor));
        // ambient stuff
        ambientStrAddress = glGetUniformLocation(currShader, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        ambientColorAddress = glGetUniformLocation(currShader, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        cameraPosAddress = glGetUniformLocation(currShader, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(camera.getCameraPos()));

        specStrAddress = glGetUniformLocation(currShader, "specStr");
        glUniform1f(specStrAddress, specStr);

        specPhongAddress = glGetUniformLocation(currShader, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        projLoc = glGetUniformLocation(currShader, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        viewLoc = glGetUniformLocation(currShader, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getViewMatrix()));

        transformLoc = glGetUniformLocation(currShader, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(ship.getTransMatrix()));

        glDrawArrays(GL_TRIANGLES, 0, ship.getVertexData().size() / 8);

        /*
        *
        *
        *
        *               3nd Obj render (chest)
        *
        *
        *
        */

        glBindVertexArray(chest.getVAO());
        chest.loadModel(-13.0f, -148.0f, -142.0f, scale_x, rot_x, rot_y, rot_z, theta);

        tex0Address = glGetUniformLocation(currShader, "tex0");
        glBindTexture(GL_TEXTURE_2D, chest.getTexture());
        glUniform1i(tex0Address, 0);

        // diffuse stuff
        lightAddress = glGetUniformLocation(currShader, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        lightColorAddress = glGetUniformLocation(currShader, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));
        sun_lightAddress = glGetUniformLocation(currShader, "sun_lightPos");
        glUniform3fv(sun_lightAddress,
            1,
            glm::value_ptr(sun_lightPos));

        sun_lightColorAddress = glGetUniformLocation(currShader, "sun_lightColor");
        glUniform3fv(sun_lightColorAddress,
            1,
            glm::value_ptr(sun_lightColor));
        // ambient stuff
        ambientStrAddress = glGetUniformLocation(currShader, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        ambientColorAddress = glGetUniformLocation(currShader, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        cameraPosAddress = glGetUniformLocation(currShader, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(camera.getCameraPos()));

        specStrAddress = glGetUniformLocation(currShader, "specStr");
        glUniform1f(specStrAddress, specStr);

        specPhongAddress = glGetUniformLocation(currShader, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        projLoc = glGetUniformLocation(currShader, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        viewLoc = glGetUniformLocation(currShader, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getViewMatrix()));

        transformLoc = glGetUniformLocation(currShader, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(chest.getTransMatrix()));

        glDrawArrays(GL_TRIANGLES, 0, chest.getVertexData().size() / 8);

        /*
        *
        *
        *
        *               3nd Obj render (jellyfish)
        *
        *
        *
        */
        
        //top right
        glBindVertexArray(jfish.getVAO());
        jfish.loadModel(39.0f, -132.0f, -112.0f, scale_x, rot_x, rot_y, rot_z, theta);

        tex0Address = glGetUniformLocation(currShader, "tex0");
        glBindTexture(GL_TEXTURE_2D, jfish.getTexture());
        glUniform1i(tex0Address, 0);

        // diffuse stuff
        lightAddress = glGetUniformLocation(currShader, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        lightColorAddress = glGetUniformLocation(currShader, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));
        sun_lightAddress = glGetUniformLocation(currShader, "sun_lightPos");
        glUniform3fv(sun_lightAddress,
            1,
            glm::value_ptr(sun_lightPos));

        sun_lightColorAddress = glGetUniformLocation(currShader, "sun_lightColor");
        glUniform3fv(sun_lightColorAddress,
            1,
            glm::value_ptr(sun_lightColor));
        // ambient stuff
        ambientStrAddress = glGetUniformLocation(currShader, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        ambientColorAddress = glGetUniformLocation(currShader, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        cameraPosAddress = glGetUniformLocation(currShader, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(camera.getCameraPos()));

        specStrAddress = glGetUniformLocation(currShader, "specStr");
        glUniform1f(specStrAddress, specStr);

        specPhongAddress = glGetUniformLocation(currShader, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        projLoc = glGetUniformLocation(currShader, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        viewLoc = glGetUniformLocation(currShader, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getViewMatrix()));

        transformLoc = glGetUniformLocation(currShader, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(jfish.getTransMatrix()));

        glDrawArrays(GL_TRIANGLES, 0, jfish.getVertexData().size() / 5);

        /*
        *
        *
        *
        *               4th Obj render (trident)
        *
        *
        *
        */

        //leftmost
        glBindVertexArray(trident.getVAO());
        trident.loadModel(-14.0f, -126.0f, -85.0f, scale_x, rot_x, rot_y, rot_z, theta);

        tex0Address = glGetUniformLocation(currShader, "tex0");
        glBindTexture(GL_TEXTURE_2D, trident.getTexture());
        glUniform1i(tex0Address, 0);

        // diffuse stuff
        lightAddress = glGetUniformLocation(currShader, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        lightColorAddress = glGetUniformLocation(currShader, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));
        sun_lightAddress = glGetUniformLocation(currShader, "sun_lightPos");
        glUniform3fv(sun_lightAddress,
            1,
            glm::value_ptr(sun_lightPos));

        sun_lightColorAddress = glGetUniformLocation(currShader, "sun_lightColor");
        glUniform3fv(sun_lightColorAddress,
            1,
            glm::value_ptr(sun_lightColor));
        // ambient stuff
        ambientStrAddress = glGetUniformLocation(currShader, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        ambientColorAddress = glGetUniformLocation(currShader, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        cameraPosAddress = glGetUniformLocation(currShader, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(camera.getCameraPos()));

        specStrAddress = glGetUniformLocation(currShader, "specStr");
        glUniform1f(specStrAddress, specStr);

        specPhongAddress = glGetUniformLocation(currShader, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        projLoc = glGetUniformLocation(currShader, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        viewLoc = glGetUniformLocation(currShader, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getViewMatrix()));

        transformLoc = glGetUniformLocation(currShader, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(trident.getTransMatrix()));

        glDrawArrays(GL_TRIANGLES, 0, trident.getVertexData().size() / 8);

        /*
        *
        *
        *
        *               5th Obj render (grey fish)
        *
        *
        *
        */

        glBindVertexArray(gfish.getVAO());
        gfish.loadModel(23.0f, -128.0f, -140.0f, scale_x, rot_x, rot_y, rot_z, theta);

        tex0Address = glGetUniformLocation(currShader, "tex0");
        glBindTexture(GL_TEXTURE_2D, gfish.getTexture());
        glUniform1i(tex0Address, 0);

        // diffuse stuff
        lightAddress = glGetUniformLocation(currShader, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        lightColorAddress = glGetUniformLocation(currShader, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));
        sun_lightAddress = glGetUniformLocation(currShader, "sun_lightPos");
        glUniform3fv(sun_lightAddress,
            1,
            glm::value_ptr(sun_lightPos));

        sun_lightColorAddress = glGetUniformLocation(currShader, "sun_lightColor");
        glUniform3fv(sun_lightColorAddress,
            1,
            glm::value_ptr(sun_lightColor));
        // ambient stuff
        ambientStrAddress = glGetUniformLocation(currShader, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        ambientColorAddress = glGetUniformLocation(currShader, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        cameraPosAddress = glGetUniformLocation(currShader, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(camera.getCameraPos()));

        specStrAddress = glGetUniformLocation(currShader, "specStr");
        glUniform1f(specStrAddress, specStr);

        specPhongAddress = glGetUniformLocation(currShader, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        projLoc = glGetUniformLocation(currShader, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getProjection()));

        viewLoc = glGetUniformLocation(currShader, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(camera.getViewMatrix()));

        transformLoc = glGetUniformLocation(currShader, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(gfish.getTransMatrix()));

        glDrawArrays(GL_TRIANGLES, 0, gfish.getVertexData().size() / 8);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    shark.clear();
    ship.clear();

    glfwTerminate();
    return 0;
}