#ifndef _CONTROL_POINT_H_
#define _CONTROL_POINT_H_

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

class ControlPoint;
class ControlHandler;

#include "BezierCurve.h"
#include "ControlHandler.h"

class ControlPoint : public Object {
public:
	const glm::vec3 scaleFacs = glm::vec3(0.2f, 0.2f, 0.2f);
	const glm::vec3 selectedColor = glm::vec3(1.0f, 0.6f, 0.0f);
	glm::vec3 point;
	Object* obj;
	glm::vec3 defCol;
	bool isAnchor;

	int useNormal, useColor;
	
	std::vector<BezierCurve*> parents;
	ControlHandler* handler;

	virtual void initializeObj() = 0;
	virtual void draw(GLuint shaderProgram, glm::mat4 C) = 0;
	virtual void selectPoint() = 0;
	virtual void deselectPoint() = 0;

	void addParent(BezierCurve* parent);
	void setHandler(ControlHandler* handle);

	void movePoint(glm::vec3 changeVec, bool fromHandler);

	void notifyParents();
	void notifyHandle(glm::vec3 changeVec, bool isAnchor);

	glm::vec3 getPoint() { return point; };
};

#endif
