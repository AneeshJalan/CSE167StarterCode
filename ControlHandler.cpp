#include "ControlHandler.h"

ControlHandler::ControlHandler(ControlPoint* t1, ControlPoint* a, ControlPoint* t2)
{
	controllers = {};
	points = {};

	controllers.push_back(t1);
	controllers.push_back(a);
	controllers.push_back(t2);

	lineWidth = 50;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	updatePoints();
}

void ControlHandler::getLinePoints()
{
	for (ControlPoint* p : controllers) {
		points.push_back(p->getPoint());
	}
}

void ControlHandler::bindPoints()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ControlHandler::updatePoints()
{
	points.clear();
	getLinePoints();
	bindPoints();
}

void ControlHandler::draw(GLuint shaderProgram, glm::mat4 C)
{
	glUseProgram(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram, "useColor"), 1);
	glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glBindVertexArray(vao);

	glLineWidth(lineWidth);

	glDrawArrays(GL_LINE_STRIP, 0, 3);

	glUniform1ui(glGetUniformLocation(shaderProgram, "useColor"), 0);

	glBindVertexArray(0);
}

void ControlHandler::adjustPoints(ControlPoint* changed, glm::vec3 changeVec, bool isAnchor)
{
	glm::vec3 reverseChange = -1.0f * changeVec;

	if (isAnchor) {
		controllers[0]->movePoint(changeVec, true);
		controllers[2]->movePoint(changeVec, true);
	}
	else {
		if (changed == controllers[0]) {
			controllers[2]->movePoint(reverseChange, true);
		}
		else {
			controllers[0]->movePoint(reverseChange, true);
		}
	}

	updatePoints();
}
