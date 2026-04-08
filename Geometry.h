#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <sstream>

#include "Object.h"
#include "Node.h"

class Geometry : public Node {
public:
	Geometry() {};
	Geometry(std::string objFilename, glm::vec3 scaleFacs = glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 getCenter();

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

	void parseFaceIndices(std::stringstream* ss, std::vector<unsigned int>* vs,
		std::vector<unsigned int>* vts, std::vector<unsigned int>* vns);

	void handleCursorMove(glm::vec3 currPos);

	void handleMouseButton(bool rotateNow);

	void rotate(float degree, glm::vec3 axis);

	glm::mat4 getModel() { return model; };
	glm::vec3 getFurthestPt() { return furthestPt; };

	glm::mat4 model;
	glm::vec3 color;
	Material material;
	GLuint modelLoc, colorLoc, matAmbLoc, matDiffLoc, matSpecLoc, matShineLoc;
	GLuint vao, vbo, vbo_normals, ebo;
	std::vector<glm::vec3> vs, vns;
	std::vector<unsigned int> face_indices;

	glm::vec3 lastPos;
	bool shouldRotate;

	glm::vec3 furthestPt;
};


#endif