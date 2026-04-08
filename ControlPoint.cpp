#include "ControlPoint.h"

void ControlPoint::addParent(BezierCurve* parent)
{
	parents.push_back(parent);
}

void ControlPoint::setHandler(ControlHandler* handle)
{
	handler = handle;
}

void ControlPoint::movePoint(glm::vec3 changeVec, bool fromHandler)
{
	point += changeVec;
	model = glm::translate(changeVec) * model;

	notifyParents();

	if (!fromHandler) {
		notifyHandle(changeVec, isAnchor);
	}
}

void ControlPoint::notifyParents()
{
	for (BezierCurve* parent : parents) {
		//std::cerr << "Notified Parent" << std::endl;
		parent->calculateCurve();
	}
}

void ControlPoint::notifyHandle(glm::vec3 changeVec, bool isAnchor)
{
	handler->adjustPoints(this, changeVec, isAnchor);
}
