#include "Plane.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

GLfloat Plane::getDistance(glm::vec3 x, bool debug)
{
	glm::vec3 x2pt = x - point;
	GLfloat distance = glm::dot(x2pt, normal);

	if (debug) std::cerr << "Distance to plane is: " << distance << std::endl;
	return /*d + */distance;
}

void Plane::setNormalAndPoint(glm::vec3 normal, glm::vec3 point)
{
	//std::cerr << "Point co-ordinates: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
	//std::cerr << "normal Dierection: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl << std::endl;
	
	setNormal(glm::normalize(normal));
	setPoint(point);
	setD();
}

void Plane::setPoint(glm::vec3 point) { 
	Plane::point = point;
	//std::cerr << "Point co-ordinates: (" << Plane::point.x << ", " << Plane::point.y << ", " << Plane::point.z << ")" << std::endl;
	setD();
}

void Plane::setNormal(glm::vec3 normal) {
	Plane::normal = glm::normalize(normal);
	// std::cerr << "Normal Direction: (" << Plane::normal.x << ", " << Plane::normal.y << ", " << Plane::normal.z << ")" << std::endl;
	setD();
}

void Plane::setD() {
	d = -(glm::dot(normal, point));
}

void Plane::rotate(glm::mat4 rotMat)
{
	setNormal(rotMat * glm::vec4(normal, 0.0f));
}

void Plane::printPlane() {
	std::cerr << "Point co-ordinates: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
	std::cerr << "normal Dierection: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl << std::endl;
}
