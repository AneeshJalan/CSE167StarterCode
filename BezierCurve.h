#ifndef _BEZIER_CURVE_H_
#define _BEZIER_CURVE_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "Object.h"
#include "Node.h"
#include "Geometry.h"

class BezierCurve;

#include "ControlPoint.h"

class BezierCurve : public Geometry {
public:
	std::vector<ControlPoint*> p;
	glm::vec3 a, b, c, d;
	GLuint tesselationLevel;
	std::vector<glm::vec3> points;
	GLuint lineWidth;

	GLfloat curveLen;

	GLuint vao, vbo;

	bool changed;

	BezierCurve(ControlPoint* p0, ControlPoint* p1, ControlPoint* p2, ControlPoint* p3, GLuint tessLevel);

	void calculateCurve();

	void calculateCoeffs();

	void calculatePoints();

	void calculateCurveLength();

	void bindPoints();

	bool verifyCurvePoints();

	glm::vec3 getPoint(GLfloat t);

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C) {};

	void handleCursorMove(glm::vec3 currPos) {};

	void handleMouseButton(bool rotateNow) {};

	void rotate(float degree, glm::vec3 axis) {};
};

#endif
