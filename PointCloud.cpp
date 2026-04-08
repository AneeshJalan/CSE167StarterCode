#include "PointCloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

PointCloud::PointCloud(std::string objFilename, GLfloat pointSize, GLfloat scaleFac) 
	: pointSize(pointSize)
{
	//scaleFac = 1;
	rotAngle = 0;
	shouldRotate = false;

	points = {};
	normals = {};
	indices = {};

	std::ifstream objFile(objFilename); // The obj file we are reading.

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;

				// Process the point. For example, you can save it to a.
				points.push_back(point);
			}
			else if (label == "vn") {
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;

				// Process the point. For example, you can save it to a.
				normals.push_back(normal);
			}
			else if (label == "f") {
				std::string indStr;
				unsigned int index;
				std::getline(ss, indStr, '/');
				index = std::stoul(indStr)-1;
				indices.push_back(index);

				std::getline(ss, indStr, ' ');

				std::getline(ss, indStr, '/');
				index = std::stoul(indStr)-1;
				indices.push_back(index);
				
				std::getline(ss, indStr, ' ');

				std::getline(ss, indStr, '/');
				index = std::stoul(indStr)-1;
				indices.push_back(index);

				/*std::cerr << indStr << std::endl;
				std::cerr << "The number is: " << index << std::endl;*/
				// break;
			}
		}
	}
	else
	{
		std::cerr << "Can't open the file " << objFilename << std::endl;
	}

	objFile.close();


	glm::vec3 center = getCenter();

	GLfloat maxDist = 0;
	GLfloat currDist = 0;

	// Shift object
	for (glm::vec3& pt : points) {
		pt = pt - center;
		currDist = glm::length(pt);

		if (currDist > maxDist) {
			maxDist = currDist;
		}
	}

	//GLfloat scaleFac = (isObject) ? 10 : 1;

	for (glm::vec3& pt : points) {
		pt = scaleFac * pt / maxDist;
	}

	// Set the model matrix to an identity matrix. 
	model = glm::mat4(1);
	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_normals);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(),
		normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indices.size(),
		indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}

glm::vec3 PointCloud::getCenter()
{
	// Find Center of Object
	GLfloat minX, minY, maxX, maxY, minZ, maxZ;

	minX = minY = minZ = INFINITY;
	maxX = maxY = maxZ = -1 * INFINITY;

	for (glm::vec3& pt : points) {
		minX = std::fmin(minX, pt.x);
		minY = std::fmin(minY, pt.y);
		minZ = std::fmin(minZ, pt.z);

		maxX = std::fmax(maxX, pt.x);
		maxY = std::fmax(maxY, pt.y);
		maxZ = std::fmax(maxZ, pt.z);
	}

	glm::vec3 ctr;

	ctr.x = (minX + maxX) / 2;
	ctr.y = (minY + maxY) / 2;
	ctr.z = (minZ + maxZ) / 2;

	return ctr;
}

PointCloud::~PointCloud() 
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbo_normals);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void PointCloud::draw()
{
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Set point size.
	glPointSize(pointSize);
	// Draw points 
	// glDrawArrays(GL_POINTS, 0, points.size());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Draw Triangles
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}

void PointCloud::update()
{
	// Spin the cube by 1 degree.
	// spin(0.1f);
	applyMVPMatrix();
}

void PointCloud::updatePointSize(GLfloat size) 
{
	pointSize = pointSize + size;

	std::cerr << model[0][0] << "\t" << model[0][1] << "\t" << model[0][2] << "\t" << model[0][3] << "\t" << std::endl;
	std::cerr << model[1][0] << "\t" << model[1][1] << "\t" << model[1][2] << "\t" << model[1][3] << "\t" << std::endl;
	std::cerr << model[2][0] << "\t" << model[2][1] << "\t" << model[2][2] << "\t" << model[2][3] << "\t" << std::endl;
	std::cerr << model[3][0] << "\t" << model[3][1] << "\t" << model[3][2] << "\t" << model[3][3] << "\t" << std::endl << std::endl;
}

void PointCloud::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(model, glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}

void PointCloud::processFace(std::stringstream ss, std::vector<unsigned int>* indices)
{
	char* triplet;
	
	while (!ss.eofbit) {
		ss >> triplet;
		processVertexTriplet(triplet, indices);
	}
}

void PointCloud::processVertexTriplet(char* triplet, std::vector<unsigned int>* indices)
{
	char* value;

	value = strtok(triplet, " /");
	std::cerr << "First value is: " << value << std::endl;
	std::cerr << "First value number is: " << std::stoul(value) << std::endl;

	value = strtok(NULL, " /");
	std::cerr << "Second value is: " << value << std::endl;
	value = strtok(NULL, " /");
	std::cerr << "Thrid value is: " << value << std::endl;
}

void PointCloud::handleCursorMove(glm::vec3 currPos)
{
	if (shouldRotate) {
		//std::cerr << "Previous point was: (" << lastPos.x << ", " << lastPos.y << ", " << lastPos.z << ")\n";
		//std::cerr << "Current point is: (" << currPos.x << ", " << currPos.y << ", " << currPos.z << ")\n";
		//float degree = glm::acos(lastPos * currPos)[1];

		glm::vec3 vel = currPos - lastPos;
		
		float degree = glm::length(vel)*100.0f;
		// std::cerr << "Degree is: " << degree << std::endl;

		glm::vec3 axis = glm::cross(lastPos, currPos);

		// std::cerr << "Axis is: (" << axis.x << ", " << axis.y << ", " << axis.z << ")\n";

		rotate(degree, axis);
	}

	lastPos = currPos;
}

void PointCloud::handleMouseButton(bool rotateNow)
{
	shouldRotate = rotateNow;
}

void PointCloud::setMaterial(Material m)
{
	material = Material(m);
}

Material PointCloud::getMaterial()
{
	return material;
}

void PointCloud::scale(double offset)
{
	GLfloat scale = 1;
	if (offset < 0) {
		scale = 0.9f;
	}
	else{
		scale = 1.1f;
	}
	
	model = glm::scale(glm::mat4(1), glm::vec3(scale, scale, scale)) * model;
}

void PointCloud::rotate(float degree, glm::vec3 axis) {
	//glm::vec3 center = getCenter();
	//model = glm::translate(model, -center);
	model = glm::rotate(glm::mat4(1), glm::radians(degree), axis) * model;
	//model = glm::translate(glm::mat4(1), center)*model;
}

void PointCloud::translate(glm::vec3 shiftVector)
{

	// shiftVector = glm::vec3(2.0f, 3.0f, 5.0f);

	/*std::cerr << "Shift Vector is at (" << shiftVector.x << ", " << shiftVector.y << ", " << shiftVector.z << ")\n";

	std::cerr << "Before translation" << std::endl;
	std::cerr << model[0][0] << "\t" << model[0][1] << "\t" << model[0][2] << "\t" << model[0][3] << "\t" << std::endl;
	std::cerr << model[1][0] << "\t" << model[1][1] << "\t" << model[1][2] << "\t" << model[1][3] << "\t" << std::endl;
	std::cerr << model[2][0] << "\t" << model[2][1] << "\t" << model[2][2] << "\t" << model[2][3] << "\t" << std::endl;
	std::cerr << model[3][0] << "\t" << model[3][1] << "\t" << model[3][2] << "\t" << model[3][3] << "\t" << std::endl << std::endl;*/

	glm::mat4 t_mat = glm::translate(shiftVector);

	/*std::cerr << "Translation Matrix" << std::endl;
	std::cerr << t_mat[0][0] << "\t" << t_mat[0][1] << "\t" << t_mat[0][2] << "\t" << t_mat[0][3] << "\t" << std::endl;
	std::cerr << t_mat[1][0] << "\t" << t_mat[1][1] << "\t" << t_mat[1][2] << "\t" << t_mat[1][3] << "\t" << std::endl;
	std::cerr << t_mat[2][0] << "\t" << t_mat[2][1] << "\t" << t_mat[2][2] << "\t" << t_mat[2][3] << "\t" << std::endl;
	std::cerr << t_mat[3][0] << "\t" << t_mat[3][1] << "\t" << t_mat[3][2] << "\t" << t_mat[3][3] << "\t" << std::endl << std::endl;*/

	model = t_mat * model;


	/*std::cerr << "After translation" << std::endl;
	std::cerr << model[0][0] << "\t" << model[0][1] << "\t" << model[0][2] << "\t" << model[0][3] << "\t" << std::endl;
	std::cerr << model[1][0] << "\t" << model[1][1] << "\t" << model[1][2] << "\t" << model[1][3] << "\t" << std::endl;
	std::cerr << model[2][0] << "\t" << model[2][1] << "\t" << model[2][2] << "\t" << model[2][3] << "\t" << std::endl;
	std::cerr << model[3][0] << "\t" << model[3][1] << "\t" << model[3][2] << "\t" << model[3][3] << "\t" << std::endl << std::endl;*/
}

void PointCloud::moveTowardsOrigin(float distance)
{
	glm::vec3 origin2center = getCenter() - glm::vec3(0, 0, 0);

	if (glm::length(origin2center) < 5) {
		return;
	}

	GLfloat closer = (distance < 0) ? -1.1 : 1.1;

	//std::cerr << "Inside moveToOrigin method" << std::endl;
	translate(glm::normalize(origin2center) * closer);
}


void PointCloud::applyMVPMatrix()
{
	//model = glm::scale(glm::mat4(1), glm::vec3(scaleFac*2,1,1));
}

