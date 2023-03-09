#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp" //core glm functionality
#include "glm/gtc/matrix_transform.hpp" //glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp" //glm extension for computing inverse matrices
#include "glm/gtc/type_ptr.hpp" //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>
#include <irrKlang.h>
#include <time.h>
#include "SkyBox.hpp"

//#pragma comment(lib, "irrKlang.lib")

using namespace irrklang;


ISoundEngine* SoundEngine = createIrrKlangDevice();

// window
gps::Window myWindow;
GLfloat angle;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint rainlLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

GLuint cameraPosLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 3.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, +10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D finalScene;
gps::Model3D dragon;
gps::Model3D duckling;
gps::Model3D flowers;
gps::Model3D rain1;
gps::Model3D rain2;
gps::Model3D rain3;
gps::Model3D lightnings;
gps::Model3D walls;
gps::Model3D ducklingWings;
gps::Model3D lightCube;

//dragon rotation --> animation
glm::mat4 dragonMatrix;
GLfloat dragonRotation = 0.0f;
glm::mat4 fireMatrix;
GLfloat fireRotation = 0.0f;
glm::mat4 ducklingMatrix;
GLfloat ducklingRotation = 35.0f;
glm::mat4 windMatrix;
GLfloat windRotation = 0.0f;
glm::mat4 wallMatrix;
GLfloat wallRotation = 0.0f;
glm::mat4 lightningsMatrix;
GLfloat lightningsRotation = 0.0f;
glm::mat4 wingsMatrix;
GLfloat wingsRotation = 0.0f;
glm::mat4 cubeMatrix;


//skybox
std::vector<const GLchar*> skyBoxFaces;
gps::SkyBox mySkyBox;
bool lightning = false;
bool flash = true;
GLint lightningLoc;


// fog
int foginit = 0;
GLint foginitLoc;
GLfloat fogDensity = 0.05f;




glm::vec3 lastCamPos;
glm::vec3 lastCamView;
float pitchCam, yawCam;
bool firstFrameTourMode = false;


bool rainStart = false;
float auxRain = -3.5f;
glm::mat4 rainMatrix;

//duckling -> animation
glm::mat4 ducklingModel;
bool ducklingOnTheMove = false;
float ducklingSpeed = 0.001f;
glm::vec3 ducklingStart(0.03f, 0.001f, 0.4f);
glm::vec3 ducklingStop(0.03f, 0.001f, 0.65f);
glm::vec3 ducklingDirection(0.0f, 0.0f, 1.0f);
glm::vec3 ducklingCurrent(0.03f, 0.0f, 0.4f);

//liiiiiight
glm::vec3 lightPos1;
glm::vec3 lightPos2;
GLuint lightPos2Loc;
GLuint lightPos1Loc;
GLuint lightDirMatrixLoc;
GLfloat lightAngle;

// shaders
gps::Shader myBasicShader;
gps::Shader skyBoxShader;
gps::Shader lightShader;

bool mouse = true;
int retina_width, retina_height;
float lastX = retina_width / 2.0f;
float lastY = retina_height / 2.0f;
double pitch = 0.0f;
double yaw = -90.0f;
int glWindowWidth = 640;
int glWindowHeight = 480;
GLFWwindow* glWindow = NULL;
float wind;
bool lightningHappened = false;


//tour
bool tourMode = false;
float t = 0.0f;
glm::vec3 start = glm::vec3(3.03f, 5.2f, 1.4f);
glm::vec3 end = glm::vec3(-3.57, 2.158, -15.0);
glm::vec3 control = glm::vec3(0.513820, 0.212418, 1.674227);

glm::vec3 Bezier(float t, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2) {
    return (1 - t) * (P0 * (1 - t) + P1 * t) + (P1 * (1 - t) + P2 * t) * t;
}


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
    //TODO
    //for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    myBasicShader.useShaderProgram();

    //set projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    //send matrix data to shader
    GLint projLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    lightShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //set Viewport transform
    glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse)
    {
        lastX = xpos;
        lastY = ypos;
        mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    myCamera.rotate(pitch, yaw);
}


void processMovement() {

    if (tourMode == false) {
        firstFrameTourMode = true;
    }

	if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    //move up
    if (pressedKeys[GLFW_KEY_Z]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    //move down
    if (pressedKeys[GLFW_KEY_X]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    // solid
    if (pressedKeys[GLFW_KEY_2]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_3]) {

        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (pressedKeys[GLFW_KEY_1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //fog
    if (pressedKeys[GLFW_KEY_F]) {

        myBasicShader.useShaderProgram();
        foginit = 1;
        fogDensity = 0.005f;
        foginitLoc = glGetUniformLocation(myBasicShader.shaderProgram, "foginit");
        glUniform1i(foginitLoc, foginit);

    }

    // stop fog
    if (pressedKeys[GLFW_KEY_G]) {
        myBasicShader.useShaderProgram();
        foginit = 0;
        foginitLoc = glGetUniformLocation(myBasicShader.shaderProgram, "foginit");
        glUniform1i(foginitLoc, foginit);

    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightning = true;
        //rainStart = true;
        if (SoundEngine) {
            SoundEngine->play2D("thunder.wav", 0);
        }

    }
    if (pressedKeys[GLFW_KEY_R]) {
        rainStart = true;

    }
    if (pressedKeys[GLFW_KEY_T]) {
        rainStart = false;

    }

    if (pressedKeys[GLFW_KEY_V]) {
        tourMode = true;
        glfwSetCursorPosCallback(myWindow.getWindow(), NULL);
    }

    //increase the intensity of fog
    if (pressedKeys[GLFW_KEY_H]) {
        fogDensity = glm::min(fogDensity + 0.001f, 1.0f);
    }
    if (pressedKeys[GLFW_KEY_J]) {
        fogDensity = glm::max(fogDensity - 0.001f, 0.0f);
    }
    if (pressedKeys[GLFW_KEY_M]) {
        ducklingOnTheMove = false;
        ducklingCurrent = ducklingStart;
    }
    if (pressedKeys[GLFW_KEY_K]) {
        ducklingOnTheMove = true;
        if (SoundEngine) {
            SoundEngine->play2D("quack.wav", 0);
        }
    }

    if (pressedKeys[GLFW_KEY_B]) { //light angle

        lightAngle += 0.3f;
        if (lightAngle > 360.0f)
            lightAngle -= 360.0f;
        glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
    }

    

}

void initOpenGLWindow() {
    myWindow.Create(2024, 900, "OpenGL Project Core");
    
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height,
            &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap failed to load at path: " << faces[i]
                << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
        GL_CLAMP_TO_EDGE);
    return textureID;
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    finalScene.LoadModel("models/final4.obj");
    dragon.LoadModel("models/animated/dragon.obj");
    duckling.LoadModel("models/animated/duckling.obj");
    flowers.LoadModel("models/animated/flowers.obj");
    rain1.LoadModel("models/animated/rain.obj");
    rain2.LoadModel("models/animated/rain.obj");
    rain3.LoadModel("models/animated/rain.obj");
    ducklingWings.LoadModel("models/animated/ducklingWings.obj");
    //for (int i = 0; i < 100; ++i)
     //   rain[i] = gps::Model3D("models/animated/rain.obj");

    lightnings.LoadModel("models/animated/lightnings.obj");
    walls.LoadModel("models/animated/walls.obj");
    lightCube.LoadModel("models/animated/cube.obj");
    //fire.LoadModel("models/animated/fire.obj");

}

void initFaces() {
    /*skyBoxFaces.push_back("models/skybox/right.tga");
    skyBoxFaces.push_back("models/skybox/left.tga");
    skyBoxFaces.push_back("models/skybox/top.tga");
    skyBoxFaces.push_back("models/skybox/bottom.tga");
    skyBoxFaces.push_back("models/skybox/back.tga");
    skyBoxFaces.push_back("models/skybox/front.tga");*/

    skyBoxFaces.push_back("models/skybox/miramar_rt.tga");
    skyBoxFaces.push_back("models/skybox/miramar_lf.tga");
    skyBoxFaces.push_back("models/skybox/miramar_up.tga");
    skyBoxFaces.push_back("models/skybox/miramar_dn.tga");
    skyBoxFaces.push_back("models/skybox/miramar_bk.tga");
    skyBoxFaces.push_back("models/skybox/miramar_ft.tga");
}


glm::mat4 computeLightSpaceTrMatrix()
{
    const GLfloat near_plane = 1.0f, far_plane = 19.0f;
    //glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -30.0f, 30.0f, near_plane, far_plane);
    glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);

    glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 50.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
    glm::mat4 lightView = glm::lookAt(20.0f * lightDirTr, glm::vec3(0.0f), glm::vec3(0.0f, 80.0f, 0.0f));

    return lightProjection * lightView;
}

void initLight() {
    glm::vec3 pointLightPositions[] = {
            glm::vec3(-43.2f, -9.3f, 35.5f),
            //glm::vec3(104.25109f, 30.95518f, 56.20976f),
            //glm::vec3(99.54386f, 30.95518f, 56.20976f)
    };
    //glUniform3fv(glGetUniformLocation(lightShader.shaderProgram, "lightPos1"), 1, glm::value_ptr(pointLightPositions[0]));
    //glUniform3fv(glGetUniformLocation(lightShader.shaderProgram, "lightPos2"), 1, glm::value_ptr(pointLightPositions[1]));
    //glUniform3fv(glGetUniformLocation(lightShader.shaderProgram, "lightPos3"), 1, glm::value_ptr(pointLightPositions[2]));
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyBoxShader.loadShader("shaders/skyboxShader.vert",
                            "shaders/skyboxShader.frag");
    lightShader.loadShader(
        "shaders/lightCube.vert",
        "shaders/lightCube.frag");

}


void initUniforms() {
	myBasicShader.useShaderProgram();

    lightDirMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDirMatrix");

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 200000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //initLight();

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    mySkyBox.Load(skyBoxFaces);
}

void lightningAction(float light) {
    myBasicShader.useShaderProgram();
    lightningLoc = glGetUniformLocation(myBasicShader.shaderProgram, "ambientStrength");
    glUniform1f(lightningLoc, light);

}


void renderTeapot(gps::Shader shader) {

    if (tourMode)
    {
        if (t < 1.0)
        {
            glm::vec3 camera_pos = Bezier(t, start, end, control);
            glm::vec3 camera_target = glm::normalize(camera_pos);
            myCamera.moveAt(camera_pos, -camera_target);
            t += 0.001f;
        }
        else
        {
            tourMode = false;
            t = 0.0f;
            glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
        }
    }
    // select active shader program
    shader.useShaderProgram();

    wind = sin(glfwGetTime()) * 0.01f;

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);

    //previewFunction();

    finalScene.Draw(myBasicShader);
    walls.Draw(shader); 

  


    
    //float wingsRotation = 0.0f;
    //glm::vec3 wingsTranslation = glm::vec3(-0.15f, 0.22f, -0.17f);


    // In your animation loop:
    //float angleScale = 0.01f; // Scale factor for wing rotation angle
    //wingsRotation = (sin(glfwGetTime()) * 20.0f * angleScale)/4; // Rotate the wings sinusoidally with scaled angle
    //wingsMatrix = glm::mat4(1.0f);
    //wingsMatrix = glm::translate(wingsMatrix, glm::vec3(0.00001f, 0.0001f, 0.0001f));
    //wingsMatrix = glm::rotate(wingsMatrix, glm::radians(angle), glm::vec3(1, 0, 0));
   /* wingsMatrix = glm::translate(wingsMatrix, glm::vec3(0.00001f, 0.0001f, 0.0001f));
    wingsMatrix = glm::rotate(wingsMatrix, glm::radians(wingsRotation), glm::vec3(1, 0, 0));
    wingsMatrix = glm::translate(wingsMatrix, glm::vec3(-0.00001f, -0.0001f, -0.0001f));*/
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wingsMatrix));
    //normalMatrix = glm::mat3(glm::inverseTranspose(view * wingsMatrix));
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    //ducklingWings.Draw(shader);
  

    if (lightning) {
        if (flash) {
            lightningAction(1.75f);
            flash = false;
            lightningHappened = true;
        }
        else {
            
            lightningAction(0.35f);
            lightning = false;
            flash = true;
        }

    }


    // DUCKLING
    //ducklingMatrix = glm::mat4(1.0f);
    //ducklingMatrix = glm::translate(ducklingMatrix, glm::vec3(0.0f, -0.2f, -0.8f));
    //ducklingMatrix = glm::scale(ducklingMatrix, glm::vec3(0.8f, 0.8f, 0.8f));
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ducklingMatrix));
    ////normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    ////glUniform3fv(cameraPosLoc, 1, glm::value_ptr(myCamera.getCameraPosition()));

    

    // draw the leaves
    windMatrix = glm::mat4(1.0f);
    windMatrix = glm::rotate(windMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
    windMatrix = glm::translate(windMatrix, glm::vec3(0.05f , 0.005f, -0.09f + (0.45 * wind)));
    //windMatrix = glm::scale(windMatrix, glm::vec3(5.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(windMatrix));
    // compute normal matrix
    //normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    flowers.Draw(shader);


    

    //DRAGON
    dragonMatrix = glm::mat4(1.0f);
    dragonMatrix = glm::rotate(dragonMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
    dragonMatrix = glm::translate(dragonMatrix, glm::vec3(-1.4f, -4.8f, 4.0f));
    dragonMatrix = glm::rotate(dragonMatrix, glm::radians(dragonRotation), glm::vec3(0, 1, 0));
    //dragonMatrix = glm::scale(dragonMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
    dragonMatrix = glm::translate(dragonMatrix, glm::vec3(2.5f, 4.5f, 2.7f));
    dragonMatrix = glm::rotate(dragonMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(dragonMatrix));
    //normalMatrix = glm::mat3(glm::inverseTranspose(view * dragonMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    dragon.Draw(shader);
    

    if (ducklingOnTheMove == true)
    {
        if (ducklingCurrent.z > ducklingStop.z){
            ducklingCurrent = ducklingStart;
        }
        else{
            ducklingCurrent = ducklingCurrent + ducklingDirection * ducklingSpeed;
        }
        
        ducklingModel = model * glm::translate(ducklingCurrent - ducklingStop);
        if (ducklingCurrent.z == ducklingStop.z) {
            ducklingOnTheMove = false;
            ducklingCurrent = ducklingStop;

        }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ducklingModel));
       
    }
    else
    {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    }
    duckling.Draw(shader);

    //send lightSpace matrix to shader
    /*glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));*/

    if (rainStart == true)
    {
        if (auxRain > -1.7f)
            auxRain -= 0.07f;
        else
            auxRain = 0.5f;

        rainMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, auxRain, -1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rainMatrix));
        //normalMatrix = glm::mat3(glm::inverseTranspose(view * rainMatrix));
       // glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        rain1.Draw(shader);

        rainMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.7f, auxRain, -0.9f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rainMatrix));
        //normalMatrix = glm::mat3(glm::inverseTranspose(view * rainMatrix));
        //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        rain2.Draw(shader);

        rainMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.8f, auxRain, -2.4f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rainMatrix));
        //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        rain3.Draw(shader);

    }

    lightShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    cubeMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    cubeMatrix = glm::translate(cubeMatrix, 1.0f * lightDir);
    cubeMatrix = glm::scale(cubeMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cubeMatrix));

    lightCube.Draw(lightShader);

    
}


void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
  

	renderTeapot(myBasicShader);
    
    skyBoxShader.useShaderProgram();
    mySkyBox.Draw(skyBoxShader, view, projection);

}


void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {


    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initFaces();
	initModels();
	initShaders();
	initUniforms();
    setWindowCallbacks();
    

    dragonRotation = 0.0f;

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();
        renderScene();
        if (dragonRotation < 360.0f) {
            dragonRotation += 3.0f;
        }
        else {
            dragonRotation = 0;
        }
        
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
