#ifndef _TANGENT_POINT_H_
#define _TANGENT_POINT_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "Object.h"
#include "PointCloud.h"
#include "Node.h"
#include "Geometry.h"
#include "ControlPoint.h"
#include "BezierCurve.h"

class TangentPoint : public ControlPoint {
public:
	TangentPoint(glm::vec3 position);

	void initializeObj();

	void draw() {};
	void update() {};
	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

	void selectPoint();
	void deselectPoint();
};

#endif