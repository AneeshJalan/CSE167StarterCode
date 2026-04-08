#include "Transform.h"

Transform::Transform(glm::mat4 translate, glm::mat4 rotate)
{
	model = translate * rotate;
	children = {};
}

void Transform::draw(GLuint shaderProgram, glm::mat4 C)
{
	glm::mat4 combinedTransform = C * model;

	/*if (!Window::insideFrustum(combinedTransform, 7.0f)) {
		return;
	}*/

	for (Node* child : children) {
		child->draw(shaderProgram, combinedTransform);
	}
}

void Transform::update(glm::mat4 C)
{
	model = C * model;
}

void Transform::addChild(Node* child)
{
	children.push_back(child);
}

void Transform::handleCursorMove(glm::vec3 currPos)
{
	if (shouldRotate) {
		//std::cerr << "Previous point was: (" << lastPos.x << ", " << lastPos.y << ", " << lastPos.z << ")\n";
		//std::cerr << "Current point is: (" << currPos.x << ", " << currPos.y << ", " << currPos.z << ")\n";
		//float degree = glm::acos(lastPos * currPos)[1];

		glm::vec3 vel = currPos - lastPos;

		float degree = glm::length(vel) * 100.0f;
		// std::cerr << "Degree is: " << degree << std::endl;

		glm::vec3 axis = glm::cross(lastPos, currPos);

		// std::cerr << "Axis is: (" << axis.x << ", " << axis.y << ", " << axis.z << ")\n";

		rotate(degree, axis, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	lastPos = currPos;
}

void Transform::handleMouseButton(bool rotateNow)
{
	shouldRotate = rotateNow;
}

void Transform::rotate(float degree, glm::vec3 axis, glm::vec3 shift) {
	//glm::vec3 center = getCenter();
	model = glm::translate(model, shift);
	model = glm::rotate(glm::mat4(1), glm::radians(degree), axis) * model;
	model = glm::translate(glm::mat4(1), -shift)*model;
}
