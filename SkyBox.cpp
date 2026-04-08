#include "SkyBox.h"

SkyBox::SkyBox(GLuint program, std::string imagePath, int boxScale)
{
	// Model matrix. Since the original size of the cube is 2, in order to
	// have a cube of some size, we need to scale the cube by size / 2.
	model = glm::scale(glm::vec3(boxScale, boxScale, boxScale));

	// The color of the cube. Try setting it to something else!
	color = glm::vec3(1.0f, 0.95f, 0.1f);

	/*
	 * Cube indices used below.
	 *    4----7
	 *   /|   /|
	 *  0-+--3 |
	 *  | 5--+-6
	 *  |/   |/
	 *  1----2
	 *
	 */

	 // The 8 vertices of a cube.

	GLfloat boxScaleFac = boxScale;
	verts = 
	{
		glm::vec3(-1, 1, 1) * boxScaleFac,
		glm::vec3(-1, -1, 1) * boxScaleFac,
		glm::vec3(1, -1, 1)* boxScaleFac,
		glm::vec3(1, 1, 1)* boxScaleFac,
		glm::vec3(-1, 1, -1)* boxScaleFac,
		glm::vec3(-1, -1, -1)* boxScaleFac,
		glm::vec3(1, -1, -1)* boxScaleFac,
		glm::vec3(1, 1, -1)* boxScaleFac
	};

	// Each ivec3(v1, v2, v3) define a triangle consists of vertices v1, v2 
	// and v3 in counter-clockwise order.
	faces = 
	{
		// Front face.
		glm::ivec3(0, 1, 2),
		glm::ivec3(2, 3, 0),
		// Back face.
		glm::ivec3(7, 6, 5),
		glm::ivec3(5, 4, 7),
		// Right face.
		glm::ivec3(3, 2, 6),
		glm::ivec3(6, 7, 3),
		// Left face.
		glm::ivec3(4, 5, 1),
		glm::ivec3(1, 0, 4),
		// Top face.
		glm::ivec3(4, 0, 3),
		glm::ivec3(3, 7, 4),
		// Bottom face.
		glm::ivec3(1, 5, 6),
		glm::ivec3(6, 2, 1),
	};

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbos);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(),
		verts.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 0. 
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(),
		verts.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 0. 
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Bind to the second VBO. We will use it to store the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	// Pass in the data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * faces.size(),
		faces.data(), GL_STATIC_DRAW);

	// Unbind from the VBOs.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	textureId = loadCubeMap(imagePath);
}

SkyBox::~SkyBox()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(2, vbos);
	glDeleteVertexArrays(1, &vao);
}

GLuint SkyBox::loadCubeMap(std::string imagePath)
{
	std::vector<std::string> textures = {};

	/*textures.push_back(imagePath + "_rt.tga");
	textures.push_back(imagePath + "_lf.tga");
	textures.push_back(imagePath + "_up.png");
	textures.push_back(imagePath + "_dn.tga");
	textures.push_back(imagePath + "_bk.tga");
	textures.push_back(imagePath + "_ft.tga");*/

	textures.push_back(imagePath + "_right.jpg");
	textures.push_back(imagePath + "_left.jpg");
	textures.push_back(imagePath + "_top.jpg");
	textures.push_back(imagePath + "_base.jpg");
	textures.push_back(imagePath + "_front.jpg");
	textures.push_back(imagePath + "_back.jpg");


	GLuint textureId;
	glGenTextures(1, &textureId); // Get unique ID for texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId); // Tell OpenGL which texture to edit

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set bi-linear interpolation
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // for both filtering modes
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture edge mode
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width, height, nrChannels;
	for (GLuint i = 0; i < textures.size(); i++)
	{
		unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
		//Image* image = loadJPG(imagePath); // load image from disk; uses third party Image library
			// Depending on the image library, the texture image may have to be flipped vertically
			// Load image into OpenGL texture in GPU memory:
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // Always GL_TEXTURE_2D for image textures
			0, // 0 for now
			GL_RGB, // Format OpenGL uses for image without alpha channel
			width, height, // Width and height
			0, // The border of the image
			GL_RGB, // GL_RGB, because pixels are stored in RGB format
			GL_UNSIGNED_BYTE, // GL_UNSIGNED_BYTE, because pixels are stored as unsigned numbers
			data); // The actual RGB image data

		stbi_image_free(data);
	}

	return textureId; // Return the ID of the texture
}

void SkyBox::spin(float deg)
{
}

void SkyBox::draw(GLuint program)
{

	/*glUseProgram(program);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glDepthMask(GL_FALSE);

	glDepthFunc(GL_LEQUAL);*/

	// Bind to the VAO.
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Draw triangles using the indices in the second VBO, which is an 
	// elemnt array buffer.
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*glDisable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	glDepthFunc(GL_LESS);*/

	// Unbind from the VAO.
	glBindVertexArray(0);

	/*
	glUseProgram(program);

	//glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Draw triangles using the indices in the second VBO, which is an 
	// elemnt array buffer.
	glDrawElements(GL_TRIANGLES, sizeof(glm::vec3) * faces.size(), GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	// Unbind from the VAO.
	glBindVertexArray(0);
	*/

}

void SkyBox::update()
{
}
