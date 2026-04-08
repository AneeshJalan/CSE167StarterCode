#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "Object.h"
#include "Cube.h"
#include "PointCloud.h"
#include "shader.h"
#include "Node.h"
#include "Plane.h"
#include <vector>

class Window
{
public:
	static int width;
	static int height;
	static const char* windowTitle;
	static Cube* cube;
	static PointCloud * cubePoints;
	static std::vector<PointCloud*> objPoints;
	static PointCloud* currentObj;
	static PointCloud* pointLight;
	static GLint mode;

	static GLuint partySize;
	static Node*** robotParty;
	static bool showBoundingSphere;
	static GLint handAngle, handShift, shiftReset;
	static GLfloat angleReset;

	static glm::vec3 lightPos;
	static glm::vec3 lightIntensity;
	static float lightAttenuation;
	static int normFac;
	static int matFac;

	static glm::mat4 debugProjection;
	static bool cullMode;
	static bool debugMode;
	static Plane** camPlanes;

	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eye, center, up;
	static GLuint program, skyboxProg;
	static GLuint projectionLoc, viewLoc, modelLoc, colorLoc;
	
	static GLuint matDiffLoc, matAmbLoc, matSpecLoc, matShineLoc, normFacLoc, matFacLoc;
	static GLuint camLoc;
	static GLuint lightPosLoc, lightIntLoc, lightAttLoc, lightColorLoc;

	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void idleCallback();
	static void displayCallback(GLFWwindow*);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 trackBallMapping(glm::vec2 point);

	static void rotateCamera(glm::vec3& center2Cam, glm::mat4& tempRot);
	static void resetCamera();
	static bool insideFrustum(glm::mat4 transform, GLfloat maxAllowedDistance, bool debug);
	static void getViewFrustumPlanes(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far, bool debug);

	static void doTheFloss(glm::vec3 armPivotOffset, glm::vec3 legPivotOffset);
	static void shakeHead();
	static void moveEyes();

	static glm::vec3 getPointOnTrack(GLuint index);
	static glm::vec3 getRiderPositionOnTrack();
	static void updateRiderPositionOntrack();
};

#endif
