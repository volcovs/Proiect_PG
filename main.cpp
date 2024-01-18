//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <Windows.h>

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 9182;
const unsigned int SHADOW_HEIGHT = 9182;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 lampLightDir;
GLuint lampLightDirLoc;
glm::vec3 lampLightColor;
GLuint lampLightColorLoc;
glm::vec3 lampLightDir2;
GLuint lampLightDirLoc2;
glm::vec3 lampLightColor2;
GLuint lampLightColorLoc2;
glm::vec3 lampLightDir3;
GLuint lampLightDirLoc3;
glm::vec3 lampLightColor3;
GLuint lampLightColorLoc3;
glm::vec3 lampLightDir4;
GLuint lampLightDirLoc4;
glm::vec3 lampLightColor4;
GLuint lampLightColorLoc4;
glm::vec3 lampLightDir5;
GLuint lampLightDirLoc5;
glm::vec3 lampLightColor5;
GLuint lampLightColorLoc5;

GLuint textureID;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

gps::Camera myCamera(
				glm::vec3(0.0f, 0.5f, 2.0f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.05f;

bool pressedKeys[1024];
float angleY = 0.0f;

float rotationAngle = 0.0f;
float zMove = 0.0f;
GLfloat lightAngle;

gps::Model3D scene;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D bottles;
gps::Model3D secondLight;
gps::Model3D otherLights;
gps::Model3D wheel;
gps::Model3D cart;
gps::Model3D secondLightCube;
gps::Model3D thirdLightCube;
gps::Model3D fourthLightCube;
gps::Model3D fifthLightCube;
gps::Model3D sixthLightCube;
gps::Model3D treeOne;
gps::Model3D forestOne;
gps::Model3D forestTwo;

//necessary for the rain effect
gps::Model3D rain; 
struct Raindrop {
	glm::vec3 position;
	glm::vec3 velocity;
};
std::vector<Raindrop> raindrops;

//contains fog, shadow, main light source (the sun) & 5 additional positional lights (lamps)
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader bottleShader;
gps::Shader fragElimination;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool wireframeView = false;
bool polygonalView = false;
//the smooth view is actually true by default
bool smoothView = true;
bool showDepthMap;

bool firstMouse = true;
float lastX = 960, lastY = 540;
float yaw, pitch = 0.0f;

bool firstRotation = true;

bool presentation = false;
bool firstStepPresentation = true;

int steps = 0;
int steps2 = 0;
int steps3 = 0;
int steps4 = 0;
bool left = true;

bool night = false;
bool isRain = false;
bool renderCubes = false;

float delta = 0;
float movementSpeed = 2;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		night = !night;
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		showDepthMap = !showDepthMap;
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {

		wireframeView = !wireframeView;

		if (wireframeView) {

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {

		polygonalView = !polygonalView;

		if (polygonalView) {

			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else {

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS) {

		smoothView = !smoothView;

		if (smoothView) {
			glShadeModel(GL_SMOOTH);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			glShadeModel(GL_FLAT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		if (presentation == false)
		{
			presentation = true;
			myCamera.setCameraPosition(glm::vec3(0.0f, 2.0f, 5.5f));
		}
		else
		{
			steps = 0;
			steps2 = 0;
			steps3 = 0;
			steps4 = 0;
			presentation = false;
		}
		Sleep(300);
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		rotationAngle += 1.0f;
		zMove += 0.006;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		isRain = !isRain;
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		renderCubes = !renderCubes;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	glm::vec3 position = myCamera.getCameraPosition();

	static bool isLeftMouseButtonPressed = false;
	float xoffset = 0.0f;
	float yoffset = 0.0f;

	//move only by dragging
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (firstMouse)
		{
			lastX = xpos + position.x;
			lastY = ypos + position.y;
			firstMouse = false;
		}
		xoffset = -(xpos - lastX);
		yoffset = -(lastY - ypos);
	} 
	else {
		//decomment to also move camera for any mouse movement
		if (firstMouse)
		{
			//lastX = xpos + position.x;
			//lastY = ypos + position.y;
			//firstMouse = false;
		}

		//xoffset = xpos - lastX;
		//yoffset = lastY - ypos;
	}

	lastX = xpos + position.x;
	lastY = ypos + position.y;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = position.x + cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = position.y + sin(glm::radians(pitch));
	direction.z = position.z + sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	myCamera.updateDirection(direction);

	glm::mat4 viewMatrix = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;		
		lightDir.x -= 0.05f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
		lightDir.x += 0.05f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, 5*cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, 5*cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, 5*cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, 5*cameraSpeed);		
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "Proiect PG", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	//glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);

	//color combination
	//glEnable(GL_BLEND);
}

void initSkybox() {
	std::vector<const GLchar*> faces;
	/*
	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");
	*/
	faces.push_back("skybox/Meadow/posx.jpg");
	faces.push_back("skybox/Meadow/negx.jpg");
	faces.push_back("skybox/Meadow/posy.jpg");
	faces.push_back("skybox/Meadow/negy.jpg");
	faces.push_back("skybox/Meadow/posz.jpg");
	faces.push_back("skybox/Meadow/negz.jpg");

	mySkyBox.Load(faces);
}

void initObjects() {
	scene.LoadModel("objects/scene/scene2.obj", 0.0f);
	secondLight.LoadModel("objects/lamp/lamp.obj", 0.0f);
	otherLights.LoadModel("objects/lamp/other_lamps.obj", 0.0f);
	ground.LoadModel("objects/ground/ground.obj", 0.0f);
	lightCube.LoadModel("objects/cube/cube.obj", 0.0f);
	secondLightCube.LoadModel("objects/cube/cube.obj", 0.0f);
	thirdLightCube.LoadModel("objects/cube/cube.obj", 0.0f);
	fourthLightCube.LoadModel("objects/cube/cube.obj", 0.0f);
	fifthLightCube.LoadModel("objects/cube/cube.obj", 0.0f);
	sixthLightCube.LoadModel("objects/cube/cube.obj", 0.0f);
	screenQuad.LoadModel("objects/quad/quad.obj", 0.0f);
	bottles.LoadModel("objects/bottles/bottles.obj", 1.0f);
	wheel.LoadModel("objects/wheel/wheel.obj", 0.0f);
	cart.LoadModel("objects/cart/cart.obj", 0.0f);
	rain.LoadModel("objects/rain/rain.obj", 0.0f);
	treeOne.LoadModel("objects/treeOne/untitled.obj", 1.0f);
	forestOne.LoadModel("objects/forestOne/forestOne.obj", 1.0f);
	forestTwo.LoadModel("objects/forestTwo/forestTwo.obj", 1.0f);
}

void initShaders() {
	//reflections
	bottleShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	bottleShader.useShaderProgram();
	//fog + shadows + multiple light sources
	myCustomShader.loadShader("shaders/shaderTest.vert", "shaders/shaderTest.frag");
	myCustomShader.useShaderProgram();
	
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/lightSpaceTrMatrix.vert", "shaders/lightSpaceTrMatrix.frag");
	depthMapShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	fragElimination.loadShader("shaders/shaderStartFragElim.vert", "shaders/shaderStartFragElim.frag");
	fragElimination.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(10.0f, 5.0f, 5.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	myCustomShader.useShaderProgram();
	//set the light direction for the second source of light
	lampLightDir = glm::vec3(-1.6f, 0.6f, -0.2f);
	lightRotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	lampLightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightDir");
	glUniform3fv(lampLightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lampLightDir));

	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "on"), 1.0f);

	//set light color
	lampLightColor = glm::vec3(1.0f, 1.0f, 0.7f); //white light
	lampLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightColor");
	glUniform3fv(lampLightColorLoc, 1, glm::value_ptr(lampLightColor));

	//second lamp
	lampLightDir2 = glm::vec3(-2.6f, 0.5f, -2.4f);
	lightRotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	lampLightDirLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightDir2");
	glUniform3fv(lampLightDirLoc2, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lampLightDir2));

	//set light color
	lampLightColor2 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lampLightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightColor2");
	glUniform3fv(lampLightColorLoc2, 1, glm::value_ptr(lampLightColor2));

	//third lamp
	lampLightDir3 = glm::vec3(-4.8f, 0.5f, -1.4f);
	lightRotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	lampLightDirLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightDir3");
	glUniform3fv(lampLightDirLoc3, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lampLightDir3));

	//set light color
	lampLightColor3 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lampLightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightColor3");
	glUniform3fv(lampLightColorLoc3, 1, glm::value_ptr(lampLightColor3));

	//fourth lamp
	lampLightDir4 = glm::vec3(2.5f, 0.5f, -0.7f);
	lightRotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	lampLightDirLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightDir4");
	glUniform3fv(lampLightDirLoc4, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lampLightDir4));

	//set light color
	lampLightColor4 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lampLightColorLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightColor4");
	glUniform3fv(lampLightColorLoc4, 1, glm::value_ptr(lampLightColor));

	//fifth lamp
	lampLightDir5 = glm::vec3(-3.8f, 0.5f, -4.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	lampLightDirLoc5 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightDir5");
	glUniform3fv(lampLightDirLoc5, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lampLightDir5));

	//set light color
	lampLightColor5 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lampLightColorLoc5 = glGetUniformLocation(myCustomShader.shaderProgram, "lampLightColor5");
	glUniform3fv(lampLightColorLoc5, 1, glm::value_ptr(lampLightColor5));
}

void initFBO() {
	//Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);

	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	const GLfloat near_plane = 0.1f;
	const GLfloat far_plane = 50.0f;

	//pentru a nu deforma imaginea
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void renderRaindrop(gps::Shader shader) {
	shader.useShaderProgram();

	for (const auto& raindrop : raindrops) {
		glm::mat4 raindropModelMatrix = glm::translate(glm::mat4(1.0f), raindrop.position);
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(raindropModelMatrix));
		rain.Draw(shader);
	}
}

void updateRaindrop(float deltaTime) {
	for (auto& raindrop : raindrops) {
		raindrop.position += raindrop.velocity;

		//check if raindrop hit the ground
		if (raindrop.position.y < -10.0f) {
			//reset raindrop position to the top
			raindrop.position = glm::vec3(
				static_cast<float>(rand() % 200 - 100),
				static_cast<float>(rand() % 50 + 50),
				static_cast<float>(rand() % 200 - 100)
			);
		}
	}
}

void initializeRain() {
	for (int i = 0; i < 2000; ++i) {
		Raindrop raindrop;
		raindrop.position = glm::vec3(
			static_cast<float>(rand() % 200 - 100),
			static_cast<float>(rand() % 50 + 50),
			static_cast<float>(rand() % 200 - 100)

		);
		//the rain will fall straight down
		raindrop.velocity = glm::vec3(0.0f, -2.0f, 0.0f);
		raindrops.push_back(raindrop);
	}
}

void drawObjects(gps::Shader shader, bool depthPass) {
	if (presentation == true)
	{
		if (firstStepPresentation == true) {
			myCamera.move(gps::MOVE_BACKWARD, 5 * 0.005);
			steps++;
			firstStepPresentation = false;
		}
		else
		{
			steps++;
			if (steps > 25)
			{
				steps2++;
				if (steps2 > 3) {
					steps3++;
					if (steps3 > 100) {
						steps4++;
						if (steps4 > 200) {
							presentation = false;
							firstStepPresentation = false;
							steps = 0;
							steps2 = 0;
							steps3 = 0;
							steps4 = 0;

							Sleep(2000);
							left = !left;
						}
						else {
							myCamera.move(gps::MOVE_FORWARD, 5 * 0.005);
						}
					}
					else {
						if (left)
							myCamera.move(gps::MOVE_LEFT, 5 * 0.005);
						else
							myCamera.move(gps::MOVE_RIGHT, 5 * 0.005);
					}
				}
				else {
					myCamera.move(gps::MOVE_UP, 5 * 0.005);
				}
			}
			else
			{
				myCamera.move(gps::MOVE_BACKWARD, 5 * 0.005);
			}
		}
	}

	Sleep(25);
		
	shader.useShaderProgram();
	
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scene.Draw(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	ground.Draw(shader);
	secondLight.Draw(shader);
	otherLights.Draw(shader);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, zMove));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	cart.Draw(shader);

	//scale, rotate and place the wheel on the correct position (the wheel is initially situated in the origin)
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	model = glm::translate(model, glm::vec3(-15.05f, 1.0f, zMove - 8.5f));
	//model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	wheel.Draw(shader);

	if (depthPass) {
		model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

		//shadows for the leaves
		treeOne.Draw(shader);
		forestOne.Draw(shader);
		//forestTwo.Draw(shader);
	}
}

void renderScene() {
	// depth maps creation pass
	// render depth map on screen - toggled with the M key

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//drawObjects(depthMapShader, false);
	drawObjects(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		if (night) {
			glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "on"), 1.0f);
		}
		else {
			glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "on"), 0.0f);
		}

		drawObjects(myCustomShader, false);

		//draw a white cube around the light
		lightShader.useShaderProgram();
		
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
		
		//draw a white cube around the lamp light
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = glm::translate(glm::mat4(1.0f), 1.0f * lampLightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		if (renderCubes) {
			secondLightCube.Draw(lightShader);
		}

		model = glm::translate(glm::mat4(1.0f), 1.0f * lampLightDir2);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		if (renderCubes) {
			thirdLightCube.Draw(lightShader);
		}

		model = glm::translate(glm::mat4(1.0f), 1.0f * lampLightDir3);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		if (renderCubes) {
			fourthLightCube.Draw(lightShader);
		}

		model = glm::translate(glm::mat4(1.0f), 1.0f * lampLightDir4);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		if (renderCubes) {
			fifthLightCube.Draw(lightShader);
		}
		
		model = glm::translate(glm::mat4(1.0f), 1.0f * lampLightDir5);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		if (renderCubes) {
			sixthLightCube.Draw(lightShader);
		}

		mySkyBox.Draw(skyboxShader, view, projection);

		bottleShader.useShaderProgram();
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(bottleShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(bottleShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(bottleShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		bottles.Draw(bottleShader);

		fragElimination.useShaderProgram();

		model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(fragElimination.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		view = myCamera.getViewMatrix();
		viewLoc = glGetUniformLocation(fragElimination.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		normalMatrixLoc = glGetUniformLocation(fragElimination.shaderProgram, "normalMatrix");
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		projectionLoc = glGetUniformLocation(fragElimination.shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		treeOne.Draw(fragElimination);
		forestOne.Draw(fragElimination);
		forestTwo.Draw(fragElimination);
	}
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

void updateDelta(double elapsedSeconds) {
	delta = delta + movementSpeed * elapsedSeconds;
}

double lastTimeStamp = glfwGetTime();

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initSkybox();
	initUniforms();
	initFBO();
	initializeRain();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		

		if (isRain) {
			updateDelta(glfwGetTime() - lastTimeStamp);
			lastTimeStamp = glfwGetTime();
			updateRaindrop(delta);
			renderRaindrop(myCustomShader);
		}

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
