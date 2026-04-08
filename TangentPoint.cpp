#include "TangentPoint.h"

TangentPoint::TangentPoint(glm::vec3 position)
{
	point = position;

	model = glm::translate(position) * glm::scale(scaleFacs);
	parents = {};

	useColor = 1;
	useNormal = 0;

	defCol = glm::vec3(0.0f, 1.0f, 0.0f);

	color = defCol;

	isAnchor = false;

	initializeObj();
}

void TangentPoint::initializeObj()
{
	obj = new PointCloud("./Objects/sphere.obj", 10, 0.2f);
	//((PointCloud*)obj)->translate(point);
}

void TangentPoint::draw(GLuint shaderProgram, glm::mat4 C) {

	glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(shaderProgram, "useColor"), useColor);
	glUniform1i(glGetUniformLocation(shaderProgram, "useNormCol"), useNormal);

	obj->draw();
}

void TangentPoint::update(glm::mat4 C)
{
}

void TangentPoint::selectPoint()
{
	/*useNormal = true;
	useColor = false;*/

	color = selectedColor;
}

void TangentPoint::deselectPoint()
{
	/*useNormal = false;
	useColor = true;*/

	color = defCol;
}