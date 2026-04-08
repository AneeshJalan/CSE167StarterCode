#ifndef _BOUNDINGBOX_H
#define _BOUNDINGBOX_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <sstream>

#include "Object.h"
#include "Node.h"
#include "Geometry.h"

class BoundingBox : public Geometry {
public:
	BoundingBox(std::string objFilename, glm::vec3 scaleFacs = glm::vec3(1.0f, 1.0f, 1.0f));

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

private:

};

#endif