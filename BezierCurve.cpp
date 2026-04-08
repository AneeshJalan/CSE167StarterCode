#include "BezierCurve.h"

BezierCurve::BezierCurve(ControlPoint* p0, ControlPoint* p1, ControlPoint* p2, ControlPoint* p3, GLuint tessLevel)
{
	p.push_back(p0);
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);

	tesselationLevel = tessLevel;
	lineWidth = 50;
	curveLen = 0;
	points = {};

	changed = false;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	calculateCurve();
}

void BezierCurve::calculateCurve()
{
	// std::cerr << "Calculating points on curve" << std::endl;
	calculateCoeffs();
	calculatePoints();
	bindPoints();
	calculateCurveLength();
}

void BezierCurve::calculateCoeffs()
{
	a = (-1.0f * p[0]->getPoint()) + (3.0f * p[1]->getPoint()) - (3.0f * p[2]->getPoint()) + p[3]->getPoint();
	b = (3.0f * p[0]->getPoint()) - (6.0f * p[1]->getPoint()) + (3.0f * p[2]->getPoint());
	c = (-3.0f * p[0]->getPoint()) + (3.0f * p[1]->getPoint());
	d = p[0]->getPoint();
}

void BezierCurve::calculatePoints()
{
	points.clear();

	for (int i = 0; i <= tesselationLevel; i++) {
		points.push_back(getPoint(i / (GLfloat)tesselationLevel));
	}
}

void BezierCurve::calculateCurveLength()
{
	GLfloat len = 0;
	for (int i = 1; i < points.size(); i++) {
		len += glm::length(points[i] - points[0]);
	}

	curveLen = len;
}

void BezierCurve::bindPoints()
{
	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 0. 
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

bool BezierCurve::verifyCurvePoints()
{
	if (glm::length(glm::cross(p[0]->getPoint(), points[0])) == 0 &&
		glm::length(glm::cross(p[3]->getPoint(), points[points.size() - 1]))) {
		return true;
	}

	return false;
}

glm::vec3 BezierCurve::getPoint(GLfloat t)
{
	return (a * t*t*t) + (b * t*t) + (c * t) + d;
}

void BezierCurve::draw(GLuint shaderProgram, glm::mat4 C)
{
	//std::cerr << "Number of points on curve is " << points.size() << std::endl;

	//std::cerr << "P1:\t(" << p1.x << ", " << p1.y << ", " << p1.z << ")" << std::endl;
	glUseProgram(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram, "useColor"), (GLuint)1);
	glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glBindVertexArray(vao);

	glLineWidth(lineWidth);

	glDrawArrays(GL_LINE_STRIP, 0, tesselationLevel+1);

	glUniform1ui(glGetUniformLocation(shaderProgram, "useColor"), 0);

	glBindVertexArray(0);

	//p[1]->draw(shaderProgram, C);

	for (ControlPoint* point : p) {
		point->draw(shaderProgram, C);
	}
}
