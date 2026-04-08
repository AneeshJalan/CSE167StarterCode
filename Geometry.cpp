#include "Geometry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Geometry::Geometry(std::string objFilename, glm::vec3 scaleFacs)
{
	// Read and load object points from file
	std::vector<glm::vec3> points = {};
	std::vector<glm::vec3> normals = {};
	std::vector<unsigned int> point_ind = {};
	std::vector<unsigned int> normal_ind = {};

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
			else if (label == "vt") {
				// Add support for textures later.
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

				// Vertex 1
				parseFaceIndices(&ss, &point_ind, NULL, &normal_ind);

				// Vertex 2
				parseFaceIndices(&ss, &point_ind, NULL, &normal_ind);

				// Vertex 3
				parseFaceIndices(&ss, &point_ind, NULL, &normal_ind);
			}
		}
	}
	else
	{
		std::cerr << "Can't open the file " << objFilename << std::endl;
	}

	objFile.close();

	for (int i = 0; i < point_ind.size(); i++) {
		vs.push_back(points[point_ind[i]]);
		vns.push_back(normals[normal_ind[i]]);
		face_indices.push_back(i);
	}

	glm::vec3 center = getCenter();

	GLfloat maxDist = 0;
	GLfloat currDist = 0;

	/*GLfloat maxX = 0, maxY = 0, maxZ = 0;
	GLfloat minX = 10, minY = 10, minZ = 10;*/

	// Shift object
	for (glm::vec3& pt : vs) {
		pt = pt - center;
		currDist = glm::length(pt);

		if (currDist > maxDist) {
			maxDist = currDist;
		}

		/*if (maxX < pt.x) {
			maxX = pt.x;
		}

		if (maxY < pt.y) {
			maxY = pt.y;
		}

		if (maxZ < pt.z) {
			maxZ = pt.z;
		}

		if (minX > pt.x) {
			minX = pt.x;
		}

		if (minY > pt.y) {
			minY = pt.y;
		}

		if (minZ > pt.z) {
			minZ = pt.z;
		}*/
	}

	glm::vec3 temp = maxDist * scaleFacs;

	/*std::cerr << "Minimum co-ordinates: (" << minX << ", " << minY << ", " << minZ << ")" << std::endl;
	std::cerr << "Maximum co-ordinates: (" << maxX << ", " << maxY << ", " << maxZ << ")" << std::endl;
	std::cerr << "Maximum distance: (" << temp.x << ", " << temp.y << ", " << temp.z << ")" << std::endl;*/

	// Set the model matrix to an identity matrix. 
	model = glm::scale(glm::mat4(1), scaleFacs);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vs.size(),
		vs.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vns.size(),
		vns.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * face_indices.size(),
		face_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);


	// Set object material (SILVER)
	material.ambient = glm::vec3(0.19225f, 0.19225f, 0.19225);
	material.diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754);
	material.specular = glm::vec3(0.508273f, 0.508273f, 0.508273);
	material.shininess = 0.4;
}

glm::vec3 Geometry::getCenter()
{
	// Find Center of Object
	GLfloat minX, minY, maxX, maxY, minZ, maxZ;

	minX = minY = minZ = INFINITY;
	maxX = maxY = maxZ = -1 * INFINITY;

	for (glm::vec3& pt : vs) {
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

void Geometry::draw(GLuint shaderProgram, glm::mat4 C)
{

	glm::mat4 obj2world = C * model;

	// Set shader to be used.
	glUseProgram(shaderProgram);

	// Get location of Uniforms
	modelLoc = glGetUniformLocation(shaderProgram, "model");
	colorLoc = glGetUniformLocation(shaderProgram, "color");
	matAmbLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	matDiffLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	matSpecLoc = glGetUniformLocation(shaderProgram, "material.specular");
	matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj2world));
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));

	glUniform3fv(matAmbLoc, 1, glm::value_ptr(material.ambient));
	glUniform3fv(matDiffLoc, 1, glm::value_ptr(material.diffuse));
	glUniform3fv(matSpecLoc, 1, glm::value_ptr(material.specular));
	glUniform1f(matShineLoc, material.shininess);

	// Bind to the VAO.
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Draw Triangles
	glDrawElements(GL_TRIANGLES, face_indices.size(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}

void Geometry::update(glm::mat4 C)
{
	//glm::vec3 yAxis(1.0f, 0.0f, 0.0f);
	//model = C * glm::rotate(glm::mat4(1), glm::radians(1.0f), yAxis) * model;
	model = C * model;
}

void Geometry::parseFaceIndices(std::stringstream* ss, std::vector<unsigned int>* vs, std::vector<unsigned int>* vts, std::vector<unsigned int>* vns)
{
	*ss << ' ';

	std::string indStr;
	unsigned int index;

	// Get vertex index
	std::getline(*ss, indStr, '/');
	index = std::stoul(indStr) - 1;
	vs->push_back(index);

	// Get vertex texture index
	std::getline(*ss, indStr, '/');
	if (indStr.length() > 0)
	{
		index = std::stoul(indStr) - 1;
		if (vts) vts->push_back(index);
	}

	// Get vertex normal index
	std::getline(*ss, indStr, ' ');
	if (indStr.length() > 0) {
		index = std::stoul(indStr) - 1;
		if (vns) vns->push_back(index);
	}
}


void Geometry::handleCursorMove(glm::vec3 currPos)
{
	if (shouldRotate) {
		//std::cerr << "Previous point was: (" << lastPos.x << ", " << lastPos.y << ", " << lastPos.z << ")\n";
		//std::cerr << "Current point is: (" << currPos.x << ", " << currPos.y << ", " << currPos.z << ")\n";
		//float degree = glm::acos(lastPos * currPos)[1];

		glm::vec3 vel = currPos - lastPos;

		float degree = glm::length(vel) * 100.0f;
		// std::cerr << "Degree is: " << degree << std::endl;

		glm::vec3 axis = glm::cross(lastPos, currPos);

		// std::cerr << "Axis is: (" << axis.x << ", " << axis.y << ", " << axis.z << ")\n";

		rotate(degree, axis);
	}

	lastPos = currPos;
}

void Geometry::handleMouseButton(bool rotateNow)
{
	shouldRotate = rotateNow;
}

void Geometry::rotate(float degree, glm::vec3 axis) {
	//glm::vec3 center = getCenter();
	//model = glm::translate(model, -center);
	model = glm::rotate(glm::mat4(1), glm::radians(degree), axis) * model;
	//model = glm::translate(glm::mat4(1), center)*model;
}
