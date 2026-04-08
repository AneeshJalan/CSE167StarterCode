#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include <string>

#include "stb_image.h"

#include "Object.h"

class SkyBox : public Object
{
private:
	std::vector<glm::vec3> verts;
	std::vector<glm::ivec3> faces;

	GLuint vao;
	GLuint vbos[2];
	GLuint ebo;

	GLuint textureId;
public:
	SkyBox(GLuint program, std::string imagePath, int boxScale);
	~SkyBox();

	void draw() {};
	void draw(GLuint program);
	void update();

	GLuint loadCubeMap(std::string imagePath);

	void spin(float deg);
};

#endif

