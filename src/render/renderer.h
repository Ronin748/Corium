//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_RENDERER_H
#define CORIUM_RENDERER_H

#include "corium.h"
#include "render/shader/shader.h"
#include "texture.h"

const int BATCH_SIZE = GL_MAX_ELEMENTS_VERTICES;

class Light {
public:
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 col = glm::vec3(0.0f, 0.0f, 0.0f); // if col == (0, 0, 0) light is off

	GLuint depthMapFBO = -1;
	GLuint depthCubemap;

	GLuint w = 512;
	GLuint h = 512;

	void initShadows(GLuint new_w = 512, GLuint new_h = 512)
	{
		if (depthMapFBO == -1) {
			w = new_w;
			h = new_h;

			glGenFramebuffers(1, &depthMapFBO);

			glGenTextures(1, &depthCubemap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

			for (GLuint i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT,
							 GL_FLOAT, 0);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
			/*glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_NEGATIVE_X, depthCubemap, 0, 0);
			   glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, depthCubemap, 0, 0);
			   glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, depthCubemap, 0, 0);

			   glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_POSITIVE_X, depthCubemap, 0, 0);
			   glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_POSITIVE_Y, depthCubemap, 0, 0);
			   glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_POSITIVE_Z, depthCubemap, 0, 0);*/

			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void clearWith(float d)
	{
		glBindBuffer(GL_FRAMEBUFFER, depthMapFBO);

		glClearDepth(d);

		glClear(GL_DEPTH_BUFFER_BIT);

		glClearDepth(1.0f);         // set back to 1
	}

	~Light()
	{
		//glDeleteTextures(1, &depthCubemap);
		//glDeleteBuffers(1, &depthMapFBO);
	}

	Light()
	{

	}
};

class Renderer {
public:
	GLuint vertexArrayID;
	GLuint vertexBuffer;
	GLuint texBuffer;
	GLuint indexBuffer;
	GLuint normalBuffer;
	GLuint colBuffer;

	int vertices = 0;

	Texture texture;

	GLuint SCREEN_WIDTH = 512, SCREEN_HEIGHT = 300;

	float farPlane = 200.0f;
	glm::vec3 clearCol = glm::vec3(0.0f, 0.0f, 0.0f);

	int rfbo = 0; // render fbo, 0 internal, 1 screen

	bool shadows = true; // enable shadows => col * ds

	float amb = 0.5f;
	float lit = 0.5f;
	float renderDist = 0.5f;

	bool dithering = true;

	bool drawLines = false;

	glm::vec3 camStart = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 down = glm::vec3(0.0f, -1.0f, 0.0f);

	Shader mainShader;
	Shader depthShader;
	Shader screenShader;

	GLuint screenTex;
	GLuint screenFBO;

	GLuint screenDepthTex;

	GLuint quadBuf;
	GLuint indBuf;

	int MAX_LIGHTS = 8;
	int max_lights = 8;

	GLuint SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;

	glm::mat4 cMatrix = glm::mat4(1.0f);
	glm::mat4 pMatrix = glm::mat4(1.0f);
	glm::mat4 uMatrix = glm::mat4(1.0f);

	glm::vec3 camPos = glm::vec3(0.0f);
	glm::vec3 camRot = glm::vec3(0.0f);
	float camZ = 7.0f;

	float vertexArray[BATCH_SIZE];
	float texArray[BATCH_SIZE];
	float colArray[BATCH_SIZE];
	float normalArray[BATCH_SIZE];
	int indexArray[BATCH_SIZE];

	int batchesPerCycle = 0;
	int drawsPerCycle = 0;

	int bufferVertices = 0;
	int bufferIndices = 0;

	int w = 512;     // viewport dimensions
	int h = 300;

	float fov = 1.5f;

	float ticks = 0.0f;

	int light_i = 0;

	std::vector<Light> lights;

	void flushBatch();

	void flushBatchFBO();

	void clear() const;

	void update();

	void renderFBO();

	void add(Texture *tex, std::vector<float> *vertexData, std::vector<float> *normalData,
			 std::vector<float> *texData,
			 std::vector<float> *colData, std::vector<int> *indexData);

	void draw(Texture *tex, std::vector<float> *vertexData, std::vector<float> *normalData, std::vector<float> *texData,
			  std::vector<float> *colData, std::vector<int> *indexData);

	void render(Texture *tex, glm::mat4 *objMatrix, std::vector<float> *vertexData, std::vector<float> *normalData,
				std::vector<float> *texData,
				std::vector<float> *colData, std::vector<int> *indexData);

	void flush();

	void flushUpdate();

	void drawShadows(std::vector<float> *vertexData, std::vector<int> *indexData);

	void renderShadows(glm::mat4 *objMatrix, std::vector<float> *vertexData, std::vector<int> *indexData);

	void flushShadows();

	void drawShadowMap();

	void addShadows(std::vector<float> *vertexData, std::vector<int> *indexData);

	void clearShadows();

	void drawLine(glm::vec3 l0, glm::vec3 l1);

	void init();

	void compileShaders();

	void destroyRenderer();

	Renderer()
	{
	}
};


#endif //CORIUM_RENDERER_H
