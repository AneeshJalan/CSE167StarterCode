#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <sstream>

#include "Object.h"

class PointCloud : public Object
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	glm::vec3 center;
	glm::vec3 lastPos;
	GLfloat scaleFac;
	float rotAngle;
	bool shouldRotate;
	Material material;
	GLuint vao, vbo, vbo_normals, ebo;
	GLfloat pointSize;
public:
	PointCloud(std::string objFilename, GLfloat pointSize, GLfloat scaleFac);
	~PointCloud();

	void draw();
	void update();

	void updatePointSize(GLfloat size);
	void spin(float deg);

	void processFace(std::stringstream ss, std::vector<unsigned int>* indices);
	void processVertexTriplet(char* triplet, std::vector<unsigned int>* indices);

	void handleCursorMove(glm::vec3 currPos);
	void handleMouseButton(bool rotateNow);
	void setMaterial(Material m);
	Material getMaterial();

	void scale(double newFac);
	void rotate(float degree, glm::vec3 axis);
	void translate(glm::vec3 shiftVector);
	void moveTowardsOrigin(float distance);

	glm::vec3 getCenter();
	void applyMVPMatrix();
};

#endif

