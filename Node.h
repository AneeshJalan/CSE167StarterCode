#ifndef _NODE_H_
#define _NODE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <sstream>

#include "Object.h"

class Node {
public:
	virtual void draw(GLuint shaderProgram, glm::mat4 C) = 0;
	virtual void update(glm::mat4 C) = 0;

	virtual glm::mat4 getModel() = 0;
	virtual glm::vec3 getFurthestPt() = 0;
};


#endif