#ifndef _CONTROL_HANDLER_H_
#define _CONTROL_HANDLER_H_

class ControlPoint;

#include "ControlPoint.h"

class ControlHandler {
	std::vector<ControlPoint*> controllers;

	std::vector<glm::vec3> points;
	GLuint lineWidth;

	GLuint vao, vbo;

public:
	ControlHandler(ControlPoint* t1, ControlPoint* a, ControlPoint*t2);

	void getLinePoints();

	void bindPoints();

	void updatePoints();

	void draw(GLuint shaderProgram, glm::mat4 C);

	void adjustPoints(ControlPoint* changed, glm::vec3 changeVec, bool isAnchor);
};

#endif