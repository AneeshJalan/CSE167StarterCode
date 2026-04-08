#include "Track.h"

Track::Track(std::vector<ControlPoint*> points, std::vector<ControlHandler*> handlers)
{
	majorPoints = points;
	this->handlers = handlers;
	selectedPt = 0;
	majorPoints[selectedPt]->selectPoint();

	tesselationLevel = 150;

	trackLen = 0;

	//std::cerr << "Points/3: " << points.size() / 3 << std::endl;
	for (int i = 0; i < points.size(); i += 3) {
		//std::cerr << "Iterating for index: " << i << std::endl;
		BezierCurve* curr = new BezierCurve(points[i], points[i + 1], points[i + 2],
			points[(i + 3)%points.size()], tesselationLevel);
		
		points[i]->addParent(curr);
		points[i + 1]->addParent(curr);
		points[i + 2]->addParent(curr);
		points[(i + 3) % points.size()]->addParent(curr);

		children.push_back(curr);
	}


	calculateTrackLength();
	//std::cerr << "Number of Bezier Curves created: " << children.size() << std::endl;
}

void Track::draw(GLuint shaderProgram, glm::mat4 C)
{
	for (BezierCurve* child : children) {
		child->draw(shaderProgram, C);
	}

	for (ControlHandler* handle : handlers) {
		handle->draw(shaderProgram, C);
	}
}

void Track::update(glm::mat4 C)
{
}

void Track::calculateTrackLength()
{
	GLfloat len = 0;
	for (BezierCurve* bc : children) {
		len += bc->curveLen;
	}

	trackLen = len;
}

void Track::moveControlPoint(glm::vec3 changeVec)
{
	majorPoints[selectedPt]->movePoint(changeVec, false);
	calculateTrackLength();
}

void Track::selectNextPt()
{
	majorPoints[selectedPt]->deselectPoint();
	selectedPt = (selectedPt + 1) % majorPoints.size();
	majorPoints[selectedPt]->selectPoint();
}

void Track::selectPrevPt()
{
	majorPoints[selectedPt]->deselectPoint();
	selectedPt = (selectedPt - 1 + majorPoints.size()) % majorPoints.size();
	majorPoints[selectedPt]->selectPoint();
}
