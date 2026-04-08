#include "Window.h"
#include <filesystem>
#include <stdlib.h>
#include <time.h>
#include "Node.h"
#include "Geometry.h"
#include "Transform.h"
#include "BoundingBox.h"
#include "SkyBox.h"
#include "BezierCurve.h"
#include "AnchorPoint.h"
#include "TangentPoint.h"
#include "Track.h"

// Constant for file locations
const char* OBJECTS_DIR = "./Objects";
const char* OBJ_EXT = ".obj";
const int OBJ_EXT_LEN = 4;

int Window::width;
int Window::height;

const char* Window::windowTitle = "GLFW Starter Project";

// Objects to display.
Cube * Window::cube;
PointCloud * Window::cubePoints;
std::vector<PointCloud*> Window::objPoints;

// The object currently displaying.
PointCloud * Window::currentObj; 

glm::mat4 Window::projection; // Projection matrix.

glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

GLuint Window::program; // The shader program id.

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::modelLoc; // Location of model in shader.
GLuint Window::colorLoc; // Location of color in shader.

glm::vec3 Window::lightPos;
glm::vec3 Window::lightIntensity;
float Window::lightAttenuation;
int Window::normFac;
int Window::matFac;

GLuint Window::matAmbLoc;
GLuint Window::matDiffLoc;
GLuint Window::matSpecLoc;
GLuint Window::matShineLoc;
GLuint Window::normFacLoc;
GLuint Window::matFacLoc;

GLuint Window::camLoc;

PointCloud* Window::pointLight;

GLuint Window::lightPosLoc;
GLuint Window::lightAttLoc;
GLuint Window::lightIntLoc;
GLuint Window::lightColorLoc;

GLint Window::mode;

GLint Window::handAngle;
GLint Window::handShift;

GLfloat Window::angleReset;
GLint Window::shiftReset;

Node*** Window::robotParty;
GLuint Window::partySize;
bool Window::showBoundingSphere;

glm::mat4 Window::debugProjection;
bool Window::debugMode;
bool Window::cullMode;
Plane** Window::camPlanes;

Material objMaterial;
Material pointLMat;

Node* robotRoot, * bodyTransform, * leftEyeTransform, * rightEyeTransform, * headTransform;
Node* leftArmTransform, * rightArmTransform, * leftLegTransform, * rightLegTransform;

Node* cullSphere;
GLfloat cullRadius;

glm::vec3 lastPos;
bool shouldRotate;

glm::mat4 objRot;

glm::mat4 camRot, camTrans;

GLuint Window::skyboxProg;	// Skybox shader program id.


SkyBox* sky;
PointCloud* rider;

std::vector<ControlPoint*> curvePoints;
std::vector<ControlHandler*> curveHandlers;

Track* testTrack;

double lastTime;
glm::vec3 currPoint;
int currCurveInd;
int nextPointIndex;

bool moveRider;

PointCloud* testing;

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	skyboxProg = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");

	// Check the shader program.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Activate the shader program.
	//glUseProgram(program);
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	colorLoc = glGetUniformLocation(program, "color");

	lightPosLoc = glGetUniformLocation(program, "light.position");
	lightAttLoc = glGetUniformLocation(program, "light.attenuation");
	lightIntLoc = glGetUniformLocation(program, "light.intensities");
	lightColorLoc = glGetUniformLocation(program, "lightColor");

	camLoc = glGetUniformLocation(program, "cameraPos");

	matAmbLoc = glGetUniformLocation(program, "material.ambient");
	matDiffLoc = glGetUniformLocation(program, "material.diffuse");
	matSpecLoc = glGetUniformLocation(program, "material.specular");
	matShineLoc = glGetUniformLocation(program, "material.shininess");
	normFacLoc = glGetUniformLocation(program, "normFac");
	matFacLoc = glGetUniformLocation(program, "matFac");

	return true;
}

bool Window::initializeObjects()
{
	debugMode = false;
	cullMode = false;

	camRot = glm::mat4(1);
	camTrans = glm::mat4(1);

	camPlanes = new Plane * [6];
	for (int i = 0; i < 6; i++) {
		// std::cerr << "Creating Plane" << std::endl;
		camPlanes[i] = new Plane();
	}


	testing = new PointCloud("../FinalProject/Objects/cylinderHighNF.obj", 10.0f, 10);

	getViewFrustumPlanes(glm::radians(60.0), (double)Window::width / (double)Window::height, 1.0, 1000.0, false);

	robotRoot = new Transform(glm::mat4(1), glm::mat4(1));
	
	bodyTransform = new Transform(glm::mat4(1), glm::mat4(1));

	leftArmTransform = new Transform(glm::translate(glm::vec3(1.0f, 0.0f, 0.5f)), glm::mat4(1));
	rightArmTransform = new Transform(glm::translate(glm::vec3(-1.0f, 0.0f, 0.5f)), glm::mat4(1));
	leftLegTransform = new Transform(glm::translate(glm::vec3(-0.625f, -4.25f, 0.0f)), glm::mat4(1));
	rightLegTransform = new Transform(glm::translate(glm::vec3(0.625f, -4.25f , 0.0f)), glm::mat4(1));

	leftEyeTransform = new Transform(glm::translate(glm::vec3(-0.5f, -0.875f, 0.6f)), glm::mat4(1));
	rightEyeTransform = new Transform(glm::translate(glm::vec3(0.5f, -0.875f, 0.6f)), glm::mat4(1));
	headTransform = new Transform(glm::translate(glm::vec3(0.0f, 3.75f, 0.0f)), glm::mat4(1));

	Node* bodyGeom = new Geometry("./Objects/robot-parts-2018/body_s.obj", glm::vec3(1.0f, 2.5f, 0.5f));
	Node* limbGeom = new Geometry("./Objects/robot-parts-2018/limb_s.obj", glm::vec3(1.0f, 4.0f, 1.0f));
	Node* eyeGeom = new Geometry("./Objects/robot-parts-2018/eyeball_s.obj", glm::vec3(2.0f, 2.0f, 2.0f));
	Node* headGeom = new Geometry("./Objects/robot-parts-2018/head_s.obj", glm::vec3(1.0f, 1.0f, 1.0f));

	cullSphere = new BoundingBox("./Objects/sphere.obj", glm::vec3(6.5f, 6.5f, 6.5f));
	cullRadius = 5.0f;

	((Transform*)robotRoot)->addChild(bodyTransform);

	((Transform*)bodyTransform)->addChild(leftArmTransform);
	((Transform*)bodyTransform)->addChild(rightArmTransform);
	/*((Transform*)bodyTransform)->addChild(leftLegTransform);
	((Transform*)bodyTransform)->addChild(rightLegTransform);*/
	
	/*((Transform*)robotRoot)->addChild(leftArmTransform);
	((Transform*)robotRoot)->addChild(rightArmTransform);*/
	((Transform*)robotRoot)->addChild(leftLegTransform);
	((Transform*)robotRoot)->addChild(rightLegTransform);


	((Transform*)robotRoot)->addChild(headTransform);
	//((Transform*)bodyTransform)->addChild(headTransform);
	((Transform*)headTransform)->addChild(leftEyeTransform);
	((Transform*)headTransform)->addChild(rightEyeTransform);

	((Transform*)bodyTransform)->addChild(bodyGeom);

	((Transform*)leftArmTransform)->addChild(limbGeom);
	((Transform*)rightArmTransform)->addChild(limbGeom);
	((Transform*)leftLegTransform)->addChild(limbGeom);
	((Transform*)rightLegTransform)->addChild(limbGeom);

	((Transform*)leftEyeTransform)->addChild(eyeGeom);
	((Transform*)rightEyeTransform)->addChild(eyeGeom);
	((Transform*)headTransform)->addChild(headGeom);

	angleReset = 2000;
	shiftReset = 6000;

	partySize = 3;
	robotParty = new Node * *[partySize];
	showBoundingSphere = false;

	for (int i = 0; i < partySize; i++) {
		robotParty[i] = new Node * [partySize];
	}
	
	for (int x = 0; x < partySize; x++) {
		for (int z = 0; z < partySize; z++) {
			robotParty[x][z] = new Transform(glm::translate(glm::vec3((x - (partySize / 2.0f)) * 2 * cullRadius, 0.0f, (z - (partySize / 2.0f)) * 2 * cullRadius)), glm::mat4(1));
			((Transform*)robotParty[x][z])->addChild(robotRoot);
			((Transform*)robotParty[x][z])->addChild(cullSphere);
		}
	}

	mode = 1;

	//pointLight = new PointCloud("./Objects/sphere.obj", 1, false);
	//pointLight->translate(glm::vec3(5.0f, 5.0f, 5.0f));

	// Material for Point Light Sphere
	Material lightMat;
	lightMat.ambient = glm::vec3(0.9f, 0.8f, 0.0f);
	lightMat.diffuse = glm::vec3(0.0f, 0.0f, 1.0f);
	lightMat.specular = glm::vec3(0.633f, 0.727811f, 0.93f);
	lightMat.shininess = 0.3f;

	//pointLight->setMaterial(lightMat);
	//lightPos = pointLight->getCenter();
	//lightAttenuation = 0.125;
	//lightIntensity = pointLight->getMaterial().ambient;

	//pointLMat = pointLight->getMaterial();

	// Create a cube of size 5.
	cube = new Cube(5.0f);

	// Create a point cloud consisting of cube vertices.
	//cubePoints = new PointCloud("foo", 10);

	// NO DIFFUSE
	Material dragon;
	dragon.ambient = glm::vec3(0.9f, 0.3f, 0.4f);
	dragon.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	dragon.specular = glm::vec3(0.992157f, 0.941176f, 0.807843f); //0.7f, 1.0f, 0.4f
	dragon.shininess = 0.2179f; // 0.5

	// NO SPECULAR
	Material bear;
	bear.ambient = glm::vec3(0.725f, 0.5f, 0.745f);
	bear.diffuse = glm::vec3(0.80392f, 0.627f, 0.725f);
	bear.specular = glm::vec3(0.0f, 0.0f, 0.0f);
	bear.shininess = 0.0f;

	// HIGH SPECULAR AND DIFFUSE
	Material bunny;
	bunny.ambient = glm::vec3(0.412f, 0.3529f, 0.8451f);
	bunny.diffuse = glm::vec3(0.392f, 0.8627f, 0.5f);
	bunny.specular = glm::vec3(0.7f, 0.76f, 0.43f);
	bunny.shininess = 0.4f;


	// Read all .obj files
	//objPoints.push_back(new PointCloud("./Objects/bunny.obj", 10, true));
	/*objPoints.push_back(new PointCloud("./Objects/bear.obj", 10, true));
	objPoints.push_back(new PointCloud("./Objects/dragon.obj", 10, true));*/
	
	//objPoints[0]->setMaterial(bunny);
	/*objPoints[1]->setMaterial(bear);
	objPoints[2]->setMaterial(dragon);*/

	//currentObj = objPoints[0];
	//objMaterial = currentObj->getMaterial();
	normFac = 1;
	matFac = 0;

	//std::cerr << "Object loaded and initialised correctly" << std::endl;

	sky = new SkyBox(skyboxProg, "SkyMaps/dock/Skybox_Water222", 500);

	rider = new PointCloud("./Objects/sphere.obj", 1, 1);
	//testPoint = glm::vec3(7, 28, 26);

	// Set random seed
	srand(time(NULL));

	curvePoints = {};
	curveHandlers = {};

	ControlPoint* t1, * t2, * a;
	ControlHandler* handle;

	for (int i = 0; i < 8; i++) {
		t1 = new TangentPoint(glm::vec3((rand() % 50) - 25, (rand() % 50) - 25, -1 * (rand() % 25)));
		// curvePoints.push_back(new AnchorPoint(glm::vec3((rand() % 50) - 25, (rand() % 50) - 25, -1 * (rand() % 25))));
		t2 = new TangentPoint(glm::vec3((rand() % 50) - 25, (rand() % 50) - 25, -1 * (rand() % 25)));

		a = new AnchorPoint((t1->getPoint() + t2->getPoint()) / 2.0f);

		handle = new ControlHandler(t1, a, t2);

		t1->setHandler(handle);
		a->setHandler(handle);
		t2->setHandler(handle);

		curvePoints.push_back(t1);
		curvePoints.push_back(a);
		curvePoints.push_back(t2);

		curveHandlers.push_back(handle);
	}

	t1 = curvePoints[0];
	curvePoints.erase(curvePoints.begin());
	curvePoints.push_back(t1);
	//testCurve = new BezierCurve(glm::vec3(-23, -12, 39), testPoint, glm::vec3(17, -38, 37), glm::vec3(62,73, -28), 100);

	//std::cerr << "Number of points created: " << curvePoints.size() << std::endl;

	testTrack = new Track(curvePoints, curveHandlers);

	currCurveInd = 0;
	currPoint = testTrack->children[currCurveInd]->points[0];
	nextPointIndex = 1;

	rider->translate(currPoint);

	moveRider = false;
	lastTime = 0;
	glfwSetTime(0);

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete cube;
	delete cubePoints;
	
	for (PointCloud* obj : objPoints) {
		delete obj;
	}

	// Delete the shader program.
	glDeleteProgram(program);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(60.0), 
		double(width) / (double)height, 1.0, 1000.0);
	
	if (camPlanes) {
		getViewFrustumPlanes(glm::radians(60.0), double(width) / (double)height, 1.0, 1000.0, false);
	}

	Window::debugProjection = glm::perspective(glm::radians(80.0),
		double(width) / (double)height, 1.0, 1000.0);
}

bool Window::insideFrustum(glm::mat4 transform, GLfloat maxAllowedDistance, bool debug)
{
	glm::vec3 currPoint = view * transform * glm::vec4(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

	if (debug) {
		std::cerr << "Allowed Distance is: " << -maxAllowedDistance << std::endl;
	}

	for (int i = 0; i < 6; i++) {
		if (camPlanes[i]->getDistance(currPoint, debug) < -maxAllowedDistance) {
			if (debug) {
				std::cerr << "Not Inside Frustum!!!!!!!!" << std::endl << std::endl;
			}
			return false;
		}
	}
	if (debug) {
		std::cerr << std::endl;
	}
	return true;
}

void Window::getViewFrustumPlanes(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far, bool debug) {
	
	glm::vec3 eyeCam = view * glm::vec4(eye, 1.0f);
	glm::vec3 centerCam = view * glm::vec4(center, 1.0f);

	if (true) {
		//std::cerr << "\n\nEye position is: (" << eyeCam.x << ", " << eyeCam.y << ", " << eyeCam.z << ")" << std::endl;
		//std::cerr << "\n\nCenter position is: (" << centerCam.x << ", " << centerCam.y << ", " << centerCam.z << ")" << std::endl;
	}

	
	glm::vec3 camDir = glm::normalize(eyeCam - centerCam);
	glm::vec3 normalUp = glm::normalize(Window::up);

	glm::vec3 camX = glm::normalize(glm::cross(normalUp, -camDir));

	glm::vec3 nearPt = - near * camDir;
	glm::vec3 farPt = - far * camDir;

	if(debug) std::cerr << "\nNear Plane!" << std::endl;
	camPlanes[0]->setNormalAndPoint(-camDir, nearPt);
	
	if (debug) std::cerr << "\nFar Plane!" << std::endl;
	camPlanes[1]->setNormalAndPoint(camDir, farPt);


	//std::cerr << "Distance of eye from near plane: " << camPlanes[0]->getDistance(eye) << std::endl;
	//std::cerr << "Distance of eye from far plane: " << camPlanes[1]->getDistance(eye) << std::endl;

	GLfloat nearH = glm::tan(fovy/2.0f) * near;
	GLfloat nearW = aspect * nearH;

	glm::vec3 leftPt = nearPt + nearW * camX;
	glm::vec3 rightPt = nearPt - nearW * camX;
	glm::vec3 topPt = nearPt + nearH * normalUp;
	glm::vec3 bottomPt = nearPt - nearH * normalUp;

	glm::vec3 leftNormal = glm::normalize(glm::cross(glm::normalize(leftPt - eyeCam), normalUp));
	glm::vec3 rightNormal = glm::normalize(glm::cross(normalUp, glm::normalize(rightPt - eyeCam)));
	glm::vec3 topNormal = glm::normalize(glm::cross(camX, topPt - eyeCam));
	glm::vec3 bottomNormal = glm::normalize(glm::cross(bottomPt - eyeCam, camX));

	if (debug) std::cerr << "\nLeft Plane!" << std::endl;
	camPlanes[2]->setNormalAndPoint(leftNormal, eyeCam);
	
	if (debug) std::cerr << "\nRight Plane!" << std::endl;
	camPlanes[3]->setNormalAndPoint(rightNormal, eyeCam);

	if (debug) std::cerr << "\nTop Plane!" << std::endl;
	camPlanes[4]->setNormalAndPoint(topNormal, eyeCam);

	if (debug) std::cerr << "\nBottom Plane!" << std::endl;
	camPlanes[5]->setNormalAndPoint(bottomNormal, eyeCam);
}

void Window::idleCallback()
{
	//cube->update();
	//// Perform any updates as necessary. 
	//// currentObj->update();

	////handAngle += 0.025;

	//handAngle += 10;

	//if (handAngle >= angleReset) {
	//	handAngle = 0;
	//}

	//handShift += 10;

	//if (handShift >= shiftReset) {
	//	handShift = 0;
	//}

	//doTheFloss(glm::vec3(0.0f, -2.16f, 0.0f), glm::vec3(0.0f, 6.41f, 0.0f));

	////shakeHead();

	////moveEyes();

	////leftArmTransform->update(glm::translate(glm::mat4(1), -handPivotOffset) * glm::rotate(glm::sin(handAngle) * 0.025f, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), handPivotOffset));
	////rightArmTransform->update(glm::translate(glm::mat4(1), -handPivotOffset) * glm::rotate(glm::sin(handAngle) * 0.025f, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), handPivotOffset));

	//// robotRoot->update(glm::mat4(1));

	if (!moveRider) {
		return;
	}

	double currTime = glfwGetTime();

	GLfloat totalDist = testTrack->trackLen;

	GLfloat reqVel = totalDist / 20000.0f;

	GLfloat targetDistance = (currTime - lastTime) * reqVel;

	//std::cerr << "\nTotal length of track is: " << totalDist << std::endl;
	//std::cerr << "Required Velocity is: " << reqVel << std::endl;
	//std::cerr << "Time elapsed since last move: " << currTime - lastTime << std::endl;
	//std::cerr << "Distance to be covered in this move: " << targetDistance << std::endl;
	
	BezierCurve* currCurve = testTrack->children[currCurveInd];
	std::vector<glm::vec3> curvePoints = currCurve->points;
	glm::vec3 prevPoint;
	glm::vec3 nextPoint = currPoint;

	while (targetDistance > 0) {
		prevPoint = nextPoint;
		nextPoint = curvePoints[nextPointIndex];

		targetDistance -= glm::length(nextPoint - prevPoint);
				
		nextPointIndex++;

		if (nextPointIndex >= curvePoints.size()) {
			currCurveInd = (currCurveInd + 1) % testTrack->children.size();
			currCurve = testTrack->children[currCurveInd];
			curvePoints = currCurve->points;
			nextPointIndex = 0;
		}
	}

	glm::vec3 changedPosition = nextPoint + ((targetDistance) / glm::length(nextPoint - prevPoint));

	rider->translate(changedPosition - currPoint);

	currPoint = changedPosition;

	lastTime = currTime;
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Specify the values of the uniform variables we are going to use.
	// glm::mat4 model = currentObj->getModel();
	// glm::vec3 color = currentObj->getColor();

	glm::mat4 model = glm::mat4(1);
	glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);

	/*glm::mat4 lightModel = pointLight->getModel();
	lightPos = lightModel * glm::vec4(pointLight->getCenter(), 1.0f);*/

	glUseProgram(program);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr((debugMode)? debugProjection : projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));
	
	/*glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	glUniform3fv(lightIntLoc, 1, glm::value_ptr(lightIntensity));
	glUniform1f(lightAttLoc, lightAttenuation);*/

	glUniform3fv(camLoc, 1, glm::value_ptr(eye));

	glUniform3fv(matAmbLoc, 1, glm::value_ptr(objMaterial.ambient));
	glUniform3fv(matDiffLoc, 1, glm::value_ptr(objMaterial.diffuse));
	glUniform3fv(matSpecLoc, 1, glm::value_ptr(objMaterial.specular));
	glUniform1f(matShineLoc, objMaterial.shininess);
	
	glUniform1f(normFacLoc, normFac);
	glUniform1f(matFacLoc, matFac);

	//glUseProgram(skyboxProg);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//cube->draw();

	//glDisable(GL_CULL_FACE);

	// Render the object.
	//currentObj->draw();
	//robotRoot->draw(program, glm::mat4(1));

	int robotsVisible = 0;
	
	//for (int x = 0; x < partySize; x++) {
	//	for (int z = 0; z < partySize; z++) {
	//		//if (true) {
	//		if (!cullMode || insideFrustum(robotParty[x][z]->getModel(), cullRadius, false)) {
	//			//std::cerr << "Robot at\tx: " << x << "\tz: " << z << std::endl;
	//			robotsVisible += 1;
	//			robotParty[x][z]->draw(program, glm::mat4(1));
	//		}
	//	}
	//}

	//std::cerr << "Number of robots visible: " << robotsVisible << std::endl;

	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));

	glUniform3fv(matAmbLoc, 1, glm::value_ptr(pointLMat.ambient));
	glUniform3fv(matDiffLoc, 1, glm::value_ptr(pointLMat.diffuse));
	glUniform3fv(matSpecLoc, 1, glm::value_ptr(pointLMat.specular));
	glUniform1f(matShineLoc, pointLMat.shininess);
	glUniform1f(normFacLoc, 0.0f);
	glUniform1f(matFacLoc, 1.0f);

	//pointLight->draw();

	//cube->draw();

	glUniform1i(glGetUniformLocation(program, "useColor"), 0);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rider->getModel()));
	glFrontFace(GL_CCW);
	//rider->draw();

	//testCurve->draw(program, glm::mat4(1));

	//testTrack->draw(program, glm::mat4(1));

	testing->draw();

	glUseProgram(skyboxProg);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	//glDepthMask(GL_FALSE);

	//glDepthFunc(GL_LEQUAL);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProg, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(skyboxProg, "view"), 1, GL_FALSE, glm::value_ptr(view));
	//sky->draw(skyboxProg);

	glDisable(GL_CULL_FACE);
	//glDepthMask(GL_TRUE);

	//glDepthFunc(GL_LESS);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	

	bool move = false;
	glm::vec3 changeVec = glm::vec3(0.0f, 0.0f, 0.0f);

	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_1:
			// Set currentObj to cube
			// currentObj = cube;
			mode = 1;
			break;
		case GLFW_KEY_2:
			// Set currentObj to cubePoints
			// currentObj = cubePoints;
			mode = 2;
			break;
		case GLFW_KEY_3:
			mode = 3;
			break;
		case GLFW_KEY_F1:
			currentObj = objPoints[0];
			break;
		case GLFW_KEY_F2:
			currentObj = objPoints[1];
			break;
		case GLFW_KEY_F3:
			currentObj = objPoints[2];
			break;
		case GLFW_KEY_P:
		{
			//GLfloat sizeChange = 10;

			//// Check is Shift is not pressed
			//if ( !(mods & GLFW_MOD_SHIFT) ) {
			//	sizeChange *= -1;
			//}
			//// Update point size
			//((PointCloud*)currentObj)->updatePointSize(sizeChange);
			//normFac++;

			//std::cerr << "\n\nEye position is: (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;

			//std::cerr << "\nPlanes are:\n" << std::endl;
			//for (int i = 0; i < 6; i++) {
			//	camPlanes[i]->printPlane();
			//}

			//std::cerr << "\nParty Size is : " << partySize << std::endl;
			//for (int x = 0; x < partySize; x++) {
			//	for (int z = 0; z < partySize; z++) {
			//		glm::vec3 pos = robotParty[x][z]->getModel() * glm::vec4(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
			//		std::cerr << "\nRobot at (" << (x - (partySize / 2.0f)) << ", " << (z - (partySize / 2.0f)) << ")" << std::endl;
			//		std::cerr << "Robot Position is at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
			//		insideFrustum(robotParty[x][z]->getModel(), cullRadius, true);
			//	}
			//}

			/*Object::printVector(testTrack->majorPoints[testTrack->selectedPt]->getPoint(),
				"\nSelected Point is:\t\t");

			std::vector<BezierCurve*> parents = testTrack->majorPoints[testTrack->selectedPt]->parents;

			for (int i = 0; i < parents.size(); i++) {
				std::cerr << "\tParent number " << i << std::endl;
				Object::printVector(parents[i]->a, "\t\ta: ");
				Object::printVector(parents[i]->b, "\t\tb: ");
				Object::printVector(parents[i]->c, "\t\tc: ");
				Object::printVector(parents[i]->d, "\t\td: ");

				std::cerr << "\n\t\tPoints in the curve" << std::endl;

				for (int j = 0; j < parents[i]->points.size(); j++) {
					Object::printVector(parents[i]->points[j], "\t\t\t");
				}
			}*/

			moveRider = !moveRider;

			if (moveRider) {
				glfwSetTime(0);
				lastTime = 0;
			}

			break;
		}
		case GLFW_KEY_N:
		{
			//std::cerr << "Original value: " << normFac << std::endl;
			/*normFac = (normFac+1)%2;
			matFac = (matFac + 1) % 2;*/
			//std::cerr << "Flipped value: " << normFac << std::endl << std::endl;
			break;
		}
		case GLFW_KEY_R:
		{
			resetCamera();
			break;
		}
		case GLFW_KEY_B:
		{
			showBoundingSphere = !showBoundingSphere;
			break;
		}
		case GLFW_KEY_D:
		{
			debugMode = !debugMode;
			break;
		}
		case GLFW_KEY_C:
		{
			cullMode = !cullMode;
			break;
		}
		case GLFW_KEY_J:
		{
			std::cerr << "Points of: " << std::endl;
			for (int i = 0; i < testTrack->children.size(); i++) {
				std::cerr << "\tBezier Curve " << i << ": " << std::endl;
				BezierCurve* bc = testTrack->children[i];
				for (int j = 0; j < bc->p.size(); j++) {
					std::cerr << "\t\tPoint " << j << ": " << std::endl;
					Object::printVector(bc->p[j]->getPoint(), "\t\t\t");

					std::cerr << "\t\t\tModel: " << std::endl;
					Object::print4x4Mat(bc->p[j]->obj->getModel(), "\t\t\t\t");
					//std::cerr << "\t\tGoes through Anchor Points??\t" << bc->verifyCurvePoints() << std::endl;
				}

				std::cerr << "\t\t\tFirst pt on curve: ";
				Object::printVector(bc->points[0], "\t\t\t\t");
				

				std::cerr << "\t\t\tLast pt on curve: ";
				Object::printVector(bc->points[bc->points.size() - 1], "\t\t\t\t");
			}
			std::cerr << std::endl << std::endl;
			break;
		}
		case GLFW_KEY_X:
		{
			move = true;
			changeVec.x = 1.0f;
			if (mods == GLFW_MOD_SHIFT) {
				changeVec.x *= -1;
			}
			break;
		}
		case GLFW_KEY_Y:
		{
			move = true;
			changeVec.y = 1.0f;
			if (mods == GLFW_MOD_SHIFT) {
				changeVec.y *= -1;
			}
			break;
		}
		case GLFW_KEY_Z:
		{
			move = true;
			changeVec.z = 1.0f;
			if (mods == GLFW_MOD_SHIFT) {
				changeVec.z *= -1;
			}
			break;
		}
		default:
			break;
		}

		if(move) {
			testTrack->moveControlPoint(changeVec);
		}
	}

	//objMaterial = currentObj->getMaterial();
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		shouldRotate = false;
		if (action == GLFW_PRESS) {
			shouldRotate = true;
			testTrack->selectNextPt();
		}
		//((PointCloud*)currentObj)->handleMouseButton(shouldRotate);
		//((Transform*)robotRoot)->handleMouseButton(shouldRotate);
		//pointLight->handleMouseButton(shouldRotate);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			testTrack->selectPrevPt();
		}
	}
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (mode == 1 || mode == 3)
	{
		//((PointCloud*)currentObj)->handleCursorMove(trackBallMapping(glm::vec2(xpos, ypos)));

		//((Transform*)robotRoot)->handleCursorMove(trackBallMapping(glm::vec2(xpos, ypos)));
	}

	if (mode == 2 || mode == 3)
	{
		//pointLight->handleCursorMove(trackBallMapping(glm::vec2(xpos, ypos)));
	} 

	glm::vec3 currPos = trackBallMapping(glm::vec2(xpos, ypos));

	if (shouldRotate) {
		//std::cerr << "Previous point was: (" << lastPos.x << ", " << lastPos.y << ", " << lastPos.z << ")\n";
		//std::cerr << "Current point is: (" << currPos.x << ", " << currPos.y << ", " << currPos.z << ")\n";
		float degree = glm::acos(glm::dot(lastPos, currPos));

		//glm::vec3 vel = lastPos - currPos;

		//float degree = glm::length(vel) * 50.0f;
		// std::cerr << "Degree is: " << degree << std::endl;

		glm::vec3 axis = glm::normalize(glm::cross(lastPos, currPos));

		// std::cerr << "Axis is: (" << axis.x << ", " << axis.y << ", " << axis.z << ")\n";

		glm::vec3 center2Cam = Window::eye - Window::center;

		glm::mat4 tempRot = glm::rotate(glm::mat4(1), degree, axis);
		camRot = tempRot * camRot;
		rotateCamera(center2Cam, tempRot);

		// getViewFrustumPlanes(glm::radians(60.0), (double)Window::width / (double)Window::height, 1.0, 1000.0);
		/*std::cerr << "Up vector is: " << Window::up.x << ", " << Window::up.y << ", " << Window::up.z << ")" << std::endl;
		std::cerr << "Center point is: " << Window::center.x << ", " << Window::center.y << ", " << Window::center.z << ")" << std::endl;*/
	}

	lastPos = currPos;
}

void Window::rotateCamera(glm::vec3& center2Cam, glm::mat4& tempRot)
{
	//std::cerr << "\n\nRotating Camera Now!!" << std::endl;
	Window::center = glm::translate(glm::mat4(1), center2Cam) * tempRot * glm::translate(glm::mat4(1), -center2Cam)
		* glm::vec4(Window::center, 1.0f);
	Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

	/*for (int i = 0; i < 6; i++) {
		camPlanes[i]->rotate(tempRot);
	}*/

	getViewFrustumPlanes(glm::radians(60.0), (double)Window::width / (double)Window::height, 1.0, 1000.0, false);
}

void Window::resetCamera() {
	glm::mat4 invCamRot = glm::transpose(camRot);
	glm::mat4 invCamTrans = -camTrans;
	glm::vec3 center2Cam = Window::eye - Window::center;
	center = Window::center = glm::translate(glm::mat4(1), -center2Cam) * invCamRot * glm::translate(glm::mat4(1), center2Cam)
		* glm::vec4(Window::center, 1.0f);
	eye = invCamTrans * glm::vec4(eye, 1.0f);
	camTrans = camRot = glm::mat4(1);
}

void Window::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (mode == 1 || mode == 3)
	{
		//((PointCloud*)currentObj)->scale(yoffset);
	}

	//std::cerr << "Inside Scroll Callback" << std::endl;

	if (mode == 2 || mode == 3) {
		//glm::mat4 lightModel = pointLight->getModel();
		//glm::vec4 lightCenter_o = glm::vec4(pointLight->getCenter(), 1.0f);

		//glm::vec4 lightCenter_w = lightModel * lightCenter_o;

		//if (yoffset < 0 && glm::length(lightCenter_w) < 5) {
			//return;
		//}

		GLfloat fac = (yoffset < 0) ? -1.1 : 1.1;

		//pointLight->translate(glm::normalize(lightCenter_w) * fac);
	}

	glm::vec3 toCenter = Window::center - Window::eye;

	//std::cerr << "YOffset:\t" << yoffset << std::endl;

	if (yoffset < 0 && glm::length(toCenter) < 5.0f) {
		return;
	}

	GLfloat fac = (yoffset < 0) ? -1 : 1;

	glm::vec3 stepToCenter = glm::normalize(toCenter);
	glm::vec3 translationVec = fac * stepToCenter;
	camTrans = glm::translate(glm::mat4(1), translationVec) * camTrans;
	Window::eye = Window::eye + translationVec;

	Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

	/*for (int i = 2; i < 6; i++) {
		camPlanes[i]->setPoint(eye);
	}*/

	//std::cerr << "\n\nZooming camera Now!!!" << std::endl;
	getViewFrustumPlanes(glm::radians(60.0), (double)Window::width / (double)Window::height, 1.0, 1000.0, false);
}

glm::vec3 Window::trackBallMapping(glm::vec2 point)
{
	glm::vec3 v;
	float d;

	v.x = (2.0f * point.x - width) / width;
	v.y = (height - 2.0f * point.y) / height;
	v.z = 0.0f;

	d = glm::length(v);

	d = (d < 1.0f) ? d : 1.0f;
	v.z = sqrt(1.001f - d*d);

	v = glm::normalize(v);
	return v;
}

void Window::doTheFloss(glm::vec3 armPivotOffset, glm::vec3 legPivotOffset)
{
	glm::vec3 shift = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 noShift = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 leftShift = noShift, rightShift = noShift;
	GLfloat rotAngle = 0.0125f;

	if (glm::cos(glm::radians((handAngle / angleReset) * 360.0f)) < 0) {
		rotAngle *= -1;
	}
	else if (glm::cos(glm::radians((handAngle / angleReset) * 360.0f)) == 0) {
		rotAngle = 0;
	}

	switch (handShift)
	{
	case 500:
	{
		leftShift = -shift;
		break;
	}
	case 2500:
	{
		leftShift = shift;
		break;
	}
	case 3500:
	{
		rightShift = -shift;
		break;
	}
	case 5500:
	{
		rightShift = shift;
		break;
	}
	}

	/*if (handShift < shiftReset / 2) {
		if (((handShift / (shiftReset / 12)) - 1) % 4 == 0) {
			if (((handShift / (shiftReset / 12)) - 1) % 8 == 0) {
				leftShift = -shift;
			}
			else {
				leftShift = shift;
			}
		}
	}
	else {
		if (((handShift / (shiftReset / 12)) + 1) % 4 == 0) {
			if (((handShift / (shiftReset / 12)) + 1) % 8 == 0) {
				rightShift = -shift;
			}
			else {
				rightShift = shift;
			}
		}
	}*/

	leftArmTransform->update(glm::translate(glm::mat4(1), -armPivotOffset +leftShift) * glm::rotate(rotAngle, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), armPivotOffset));
	rightArmTransform->update(glm::translate(glm::mat4(1), -armPivotOffset +rightShift) * glm::rotate(rotAngle, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), armPivotOffset));

	leftLegTransform->update(glm::translate(glm::mat4(1), -legPivotOffset) * glm::rotate(rotAngle/4, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), legPivotOffset));
	rightLegTransform->update(glm::translate(glm::mat4(1), -legPivotOffset) * glm::rotate(rotAngle/4, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), legPivotOffset));

	bodyTransform->update(glm::rotate(-rotAngle/4, glm::vec3(0.0f, 0.0f, 1.0f)));
}


void Window::shakeHead() {
	GLfloat shiftDir = 1;

	if (glm::cos(glm::radians((handAngle / angleReset) * 360.0f)) < 0) {
		shiftDir *= -1;
	}
	
	headTransform->update(glm::translate(shiftDir * glm::vec3(-0.01f, 0.0f, 0.0f)));
}

void Window::moveEyes() {
	GLfloat shiftDir = -1;

	if (glm::tan(glm::radians(((GLfloat)handShift) / shiftReset) * 360.0f) < 0) {
		shiftDir = 1;
		std::cerr << "Changed direction of motion for eyes." << std::endl;
	}

	leftEyeTransform->update(glm::translate(shiftDir * glm::vec3(0.0f, 0.001f, 0.0f)));
	rightEyeTransform->update(glm::translate(shiftDir * glm::vec3(0.0f, 0.001f, 0.0f)));
}