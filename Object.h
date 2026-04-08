#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class Object
{
protected:
	
	glm::mat4 model;
	glm::vec3 color;
public:
	static void printVector(glm::vec3 vec, std::string tabs) {
		std::cerr << tabs;
		std::cerr << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
	};

	static void print4x4Mat(glm::mat4 mat, std::string tabs) {
		printVector(mat[0], tabs);
		printVector(mat[1], tabs);
		printVector(mat[2], tabs);
		printVector(mat[3], tabs);
	}

	glm::mat4 getModel() { return model; }
	glm::vec3 getColor() { return color; }

	void setColor(glm::vec3 col) { color = col; }
	void setModel(glm::mat4 m) { model = m; }

	virtual void draw() = 0;
	virtual void update() = 0;
};

#endif

