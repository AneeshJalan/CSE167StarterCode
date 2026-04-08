#include "BoundingBox.h"
#include "Window.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

BoundingBox::BoundingBox(std::string objFilename, glm::vec3 scaleFacs)
	: Geometry(objFilename, scaleFacs) {}

void BoundingBox::draw(GLuint shaderProgram, glm::mat4 C) {

	if (!Window::showBoundingSphere) {
		return;
	}

	//std::cerr << "Draw!\t" <<  face_indices.size() <<std::endl;
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Draw Triangles
	glDrawElements(GL_TRIANGLES, face_indices.size(), GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}

void BoundingBox::update(glm::mat4 C)
{
	Geometry::update(C);
}
