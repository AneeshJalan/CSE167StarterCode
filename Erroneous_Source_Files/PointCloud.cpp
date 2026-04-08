#include "PointCloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

PointCloud::PointCloud(std::string objFilename, GLfloat pointSize) 
	: pointSize(pointSize)
{
	points = {};
	normals = {};
	face_indices = {};

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
				// Read in a single vector normal
				glm::vec3 norm;
				ss >> norm.x >> norm.y >> norm.z;

				normals.push_back(norm);
			}
			else if (label == "f") {
				char* triplet_set;
				char* vertex_index;

				ss >> triplet_set;
				//// vertex_index = strtok(triplet_set, "/");
				//face_indices.push_back(std::stoul(triplet_set)-1);

				//ss >> triplet_set;
				//// vertex_index = strtok(triplet_set, "/");
				//face_indices.push_back(std::stoul(triplet_set) - 1);

				//ss >> triplet_set;
				//// vertex_index = strtok(triplet_set, "/");
				//face_indices.push_back(std::stoul(triplet_set) - 1);
			}
		}
	}
	else
	{
		std::cerr << "Can't open the file " << objFilename << std::endl;
	}

	objFile.close();

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

	glm::vec3 center;

	center.x = (minX + maxX) / 2;
	center.y = (minY + maxY) / 2;
	center.z = (minZ + maxZ) / 2;

	GLfloat maxDist = 0;
	glm::vec3 distPt = center;
	GLfloat currDist = 0;

	// Shift object and get maximum distance of point from center
	for (glm::vec3& pt : points) {
		pt = pt - center;
		currDist = glm::length(pt);

		if (currDist > maxDist) {
			maxDist = currDist;
			distPt = pt;
		}
	}

	// Normalize vector
	for (glm::vec3& pt : points) {
		pt = pt / maxDist;
	}

	// Set constant scale value
	GLfloat scaleFac = 12;

	for (glm::vec3& pt : points) {
		pt = scaleFac * pt;
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


	// Bind to second VBO. We will use it o store teh normals.
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	// Pass in Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(),
		normals.data(), GL_STATIC_DRAW);

	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*face_indices.size(),
		face_indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
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
	// glPointSize(pointSize);
	// Draw points 
	// glDrawArrays(GL_POINTS, 0, points.size());

	// Draw Triangles
	glDrawElements(GL_TRIANGLES, face_indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}

void PointCloud::update()
{
	// Spin the cube by 1 degree.
	// spin(0.1f);
}

void PointCloud::updatePointSize(GLfloat size) 
{
	pointSize = pointSize + size;
}

void PointCloud::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(model, glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}

