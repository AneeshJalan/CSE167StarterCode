#include "AnchorPoint.h"

AnchorPoint::AnchorPoint(glm::vec3 position)
{
	point = position;
	parents = {};

	model = glm::translate(position) * glm::scale(scaleFacs);

	useColor = 1;
	useNormal = 0;

	defCol = glm::vec3(1.0f, 0.0f, 0.0f);
	color = defCol;

	isAnchor = true;

	initializeObj();
}

void AnchorPoint::initializeObj()
{
	obj = new Cube(0.2f);
	//((Cube*)obj)->translate(point);
}

void AnchorPoint::draw(GLuint shaderProgram, glm::mat4 C) {

	glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//Object::print4x4Mat(model, "\t");
	//std::cerr << std::endl;

	glUniform1i(glGetUniformLocation(shaderProgram, "useColor"), useColor);
	glUniform1i(glGetUniformLocation(shaderProgram, "useNormCol"), useNormal);

	obj->draw();
}

void AnchorPoint::update(glm::mat4 C)
{
}

void AnchorPoint::selectPoint()
{
	/*useNormal = true;
	useColor = false;*/

	color = selectedColor;
}

void AnchorPoint::deselectPoint()
{
	/*useNormal = false;
	useColor = true;*/

	color = defCol;
}
