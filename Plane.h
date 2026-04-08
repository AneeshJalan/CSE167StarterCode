#ifndef _PLANE_H
#define _PLANE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <sstream>

#include "Object.h"
#include "Node.h"
#include "Geometry.h"

class Plane {
public:
	GLfloat getDistance(glm::vec3 x, bool debug);

	glm::vec3 getNormal() { return normal; };
	glm::vec3 getPoint() { return point; };

	void setNormal(glm::vec3 normal);
	void setPoint(glm::vec3 point);
	void setD();

	void setNormalAndPoint(glm::vec3 normal, glm::vec3 point);

	void rotate(glm::mat4 rotMat);

	void printPlane();

	glm::vec3 normal;
	glm::vec3 point;
	GLfloat d;

	glm::vec3 tl, tr, bl, br;
};

#endif