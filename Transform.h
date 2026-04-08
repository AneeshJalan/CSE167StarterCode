#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <sstream>

#include "Object.h"
#include "Node.h"
#include "Window.h"

class Transform: public Node {
public:
	Transform(glm::mat4 translate, glm::mat4 rotate);

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);
	void addChild(Node* child);

	void handleCursorMove(glm::vec3 currPos);

	void handleMouseButton(bool rotateNow);

	void rotate(float degree, glm::vec3 axis, glm::vec3 shift);

	glm::mat4 getModel() { return model; }
	glm::vec3 getFurthestPt() { return furthestPt; };

private:
	glm::mat4 model;
	std::vector<Node*> children;
	bool shouldRotate;
	glm::vec3 lastPos;

	glm::vec3 furthestPt;
};


#endif