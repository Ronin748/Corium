//
// Created by Roninkoi on 22.8.2016.
//

#ifndef CORIUM_SCREENRENDERER_H
#define CORIUM_SCREENRENDERER_H

#include "render/renderer.h"
#include "render/shader/shader.h"

class GUIRenderer {
public:
	Shader guiShader;

	GLuint *screenFBO;

	int batchnum = 0;
	int drawnum = 0;

	int rfbo = 0;

	GLuint vertexArrayID;
	GLuint vertexBuffer;
	GLuint texBuffer;
	GLuint colBuffer;
	GLuint indexBuffer;

	Texture texture;

	float vertexArray[BATCH_SIZE];
	float texArray[BATCH_SIZE];
	float colArray[BATCH_SIZE];
	int indexArray[BATCH_SIZE];

	int bufferVertices = 0;
	int bufferIndices = 0;

	glm::mat4 pMatrix = glm::mat4(1.0f);
	glm::mat4 cMatrix = glm::mat4(1.0f);
	glm::mat4 uMatrix = glm::mat4(1.0f);

	GLuint u_pMatrix;
	GLuint u_cMatrix;
	GLuint u_uMatrix;

	int w, h = 0;
	GLuint SCREEN_WIDTH = 512, SCREEN_HEIGHT = 300;

	float fov = 1.6f;

	// sync screen renderer with main renderer
	void update(Renderer *renderer)
	{
		drawnum = 0;
		batchnum = 0;

		w = renderer->w;
		h = renderer->h;

		SCREEN_WIDTH = renderer->SCREEN_WIDTH;
		SCREEN_HEIGHT = renderer->SCREEN_HEIGHT;

		screenFBO = &renderer->screenFBO;
		rfbo = renderer->rfbo;
		//fov = renderer->fov;

		uMatrix = glm::mat4(1.0f);
		cMatrix = glm::mat4(1.0f);
		pMatrix = glm::perspective(fov, (float) w / (float) h, 1.0f,
								   1000000000.0f); // lol, stretch the frustum [0.0, 1.0]
	}

	void render(Texture tex, glm::mat4 objMatrix, std::vector<float> *vertexData, std::vector<float> *texData,
				std::vector<float> *colData, std::vector<int> *indexData)
	{
		uMatrix = objMatrix;
		this->texture = tex;

		bufferIndices = indexData->size();
		bufferVertices = vertexData->size();

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexData->size() * sizeof(float), &(*vertexData)[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexData->size() * sizeof(float), &(*texData)[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexData->size() * sizeof(float), &(*colData)[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData->size() * sizeof(int), &(*indexData)[0], GL_DYNAMIC_DRAW);

		flush();
	}

	void flushBatch()
	{
		if (bufferIndices > 2) {
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), vertexArray, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
			glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), texArray, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
			glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), colArray, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices * sizeof(int), indexArray, GL_DYNAMIC_DRAW);

			flush();
		}
	}

	void draw(Texture tex, glm::mat4 objMatrix, std::vector<float> *vertexData, std::vector<float> *texData,
			  std::vector<float> *colData, std::vector<int> *indexData)
	{
		if (vertexData->size() < BATCH_SIZE && indexData->size() < BATCH_SIZE) {
			add(tex, objMatrix, vertexData, texData, colData, indexData);
		} else {
			flushBatch();
			render(tex, objMatrix, vertexData, texData, colData, indexData);
		}
	}

	void add(Texture tex, glm::mat4 objMatrix, std::vector<float> *vertexData, std::vector<float> *texData,
			 std::vector<float> *colData, std::vector<int> *indexData)
	{
		++drawnum;
		uMatrix = objMatrix;

		if (bufferVertices + vertexData->size() >= BATCH_SIZE ||
			bufferIndices + indexData->size() >= BATCH_SIZE || (tex.path != this->texture.path)) {
			flushBatch();
		}

		int vertices = bufferVertices / 4;

		for (unsigned int i = 0; i < vertexData->size(); ++i) {
			vertexArray[bufferVertices] = vertexData->at(i);
			texArray[bufferVertices] = texData->at(i);
			colArray[bufferVertices] = colData->at(i);
			++bufferVertices;
		}

		for (int i : *indexData) {
			indexArray[bufferIndices] = i + vertices;
			++bufferIndices;
		}
		this->texture = tex;
	}

	// alpha test
	bool at = true;

	void flush()
	{
		++batchnum;
		glUseProgram(guiShader.program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture.tex);

		glDisable(GL_CULL_FACE);
		if (at) {
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} else {
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		}

		if (!rfbo) {
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, *screenFBO);
		} else {
			glViewport(0, 0, w, h);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glUniformMatrix4fv(u_pMatrix, 1, GL_FALSE, glm::value_ptr(pMatrix));
		glUniformMatrix4fv(u_cMatrix, 1, GL_FALSE, glm::value_ptr(cMatrix));
		glUniformMatrix4fv(u_uMatrix, 1, GL_FALSE, glm::value_ptr(uMatrix));

		glBindVertexArray(vertexArrayID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
				0,
				4,
				GL_FLOAT,
				GL_FALSE,
				0,
				nullptr
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
		glVertexAttribPointer(
				1,
				4,
				GL_FLOAT,
				GL_FALSE,
				0,
				nullptr
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
		glVertexAttribPointer(
				2,
				4,
				GL_FLOAT,
				GL_FALSE,
				0,
				nullptr
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		glDrawRangeElements(GL_TRIANGLES, 0, bufferVertices - 1, bufferIndices, GL_UNSIGNED_INT,
							nullptr);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		bufferVertices = 0;
		bufferIndices = 0;
	}

	void init()
	{
		guiShader.init(guiShader.vertPath, guiShader.fragPath);

		glUseProgram(guiShader.program);

		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &texBuffer);
		glGenBuffers(1, &colBuffer);
		glGenBuffers(1, &indexBuffer);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		u_pMatrix = glGetUniformLocation(guiShader.program, "proj");
		u_cMatrix = glGetUniformLocation(guiShader.program, "view");
		u_uMatrix = glGetUniformLocation(guiShader.program, "model");

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void destroyRenderer()
	{
		guiShader.destroyShader();

		glDeleteVertexArrays(1, &vertexArrayID);

		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &texBuffer);
		glDeleteBuffers(1, &colBuffer);
		glDeleteBuffers(1, &indexBuffer);
	}
};

#endif //CORIUM_SCREENRENDERER_H
