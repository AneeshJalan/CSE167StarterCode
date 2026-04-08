#ifndef _TRACK_H_
#define _TRACK_H_

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
#include "BezierCurve.h"

class Track : public Geometry {
public:
	GLuint selectedPt;
	std::vector<ControlPoint*> majorPoints;
	std::vector<ControlHandler*> handlers;
	std::vector<BezierCurve*> children;

	GLuint tesselationLevel;

	GLfloat trackLen;

	Track(std::vector<ControlPoint*> points, std::vector<ControlHandler*> handlers);

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

	void calculateTrackLength();

	void moveControlPoint(glm::vec3 changeVec);
	void selectNextPt();
	void selectPrevPt();

	void handleCursorMove(glm::vec3 currPos);

	void handleMouseButton(bool rotateNow);

	void rotate(float degree, glm::vec3 axis);
};

#endif