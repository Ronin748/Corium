//
// Created by Roninkoi on 18.12.2015.
//

#include "renderer.h"

void Renderer::compileShaders()
{
	screenShader.init(screenShader.vertPath, screenShader.fragPath);
	mainShader.init(mainShader.vertPath, mainShader.fragPath);

	depthShader.initGS(depthShader.vertPath,
					   depthShader.gsPath,
					   depthShader.fragPath);
}

void Renderer::init()
{
	lightNum = MAX_LIGHTS;
	lights.resize(MAX_LIGHTS);

	compileShaders();

	glUseProgram(mainShader.program);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &texBuffer);
	glGenBuffers(1, &colBuffer);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &indexBuffer);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int l = 0; l < MAX_LIGHTS; ++l)
		lights[l].initShadows(SHADOW_WIDTH, SHADOW_HEIGHT);

	printf("SCREEN WIDTH: %s%s%s%s", &toString(SCREEN_WIDTH)[0],
		   " SCREEN HEIGHT: ", &toString(SCREEN_HEIGHT)[0], "\n");

	// SCREEN FBO
	glGenTextures(1, &screenTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glGenFramebuffers(1, &screenFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

	glGenTextures(1, &screenDepthTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, screenDepthTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT,
				 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

	gamePrint("screenTex " + toString(screenTex) + "\n");
	gamePrint("screenDepthTex " + toString(screenDepthTex) + "\n");

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, screenDepthTex, 0);

	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);

	glGenBuffers(1, &quadBuf);
	glGenBuffers(1, &indBuf);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		gamePrint("Framebuffer not complete: " + toString(fboStatus) + "\n");

	gamePrint("FBO id: " + toString(screenFBO) + "\n");
}

void Renderer::clear() const
{
	glClearColor(clearCol.r, clearCol.g, clearCol.b, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	clear();

	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
	clear();

	cMatrix = glm::mat4(1.0f);
	pMatrix = glm::mat4(1.0f);

	pMatrix = glm::perspective(fov, (float) w / (float) h, 0.05f, farPlane);

	cMatrix = glm::translate(cMatrix, glm::vec3(0.0f, 0.0f, -camZ)); // for 3rd person

	cMatrix = glm::rotate(cMatrix, camRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	cMatrix = glm::rotate(cMatrix, camRot.x, glm::vec3(cos(camRot.y), 0.0, sin(camRot.y)));

	cMatrix = glm::translate(cMatrix, camPos);
}

void Renderer::add(Texture *tex, std::vector<float> *vertexData, std::vector<float> *normalData,
				   std::vector<float> *texData,
				   std::vector<float> *colData, std::vector<int> *indexData)
{
	if (bufferVertices + vertexData->size() >= BATCH_SIZE ||
		bufferIndices + indexData->size() >= BATCH_SIZE || (tex->path != this->texture.path)) {
		flushBatch();
	}

	vertices = bufferVertices / 4;

	for (unsigned int i = 0; i < vertexData->size(); ++i) {
		vertexArray[bufferVertices] = (vertexData->at(i));
		texArray[bufferVertices] = (texData->at(i));
		colArray[bufferVertices] = (colData->at(i));
		normalArray[bufferVertices] = (normalData->at(i));
		++bufferVertices;
	}

	for (int i : *indexData) {
		indexArray[bufferIndices] = i + vertices;
		++bufferIndices;
	}

	this->texture = *tex;
}

void Renderer::draw(Texture *tex, std::vector<float> *vertexData, std::vector<float> *normalData,
					std::vector<float> *texData,
					std::vector<float> *colData, std::vector<int> *indexData)
{
	++drawsPerCycle;

	if (vertexData->size() < BATCH_SIZE && indexData->size() < BATCH_SIZE) {
		add(tex, vertexData, normalData, texData, colData, indexData);
	} else {
		flushBatch();

		render(tex, &uMatrix, vertexData, normalData, texData, colData, indexData);
	}
}

// flush batch and render FBO
void Renderer::flushBatchFBO()
{
	flushBatch();
	if (!rfbo) renderFBO();
}

void Renderer::flushUpdate()
{
	if (rfbo) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glUseProgram(mainShader.program);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);

	glUniform1f(glGetUniformLocation(mainShader.program, "farPlane"), farPlane);

	GLuint sampleTex = glGetUniformLocation(mainShader.program, "diffuseTexture");
	glUniform1i(sampleTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.tex);

	glUniform3fv(glGetUniformLocation(mainShader.program, "fogCol"), 1, &(clearCol)[0]);
	glUniform2fv(glGetUniformLocation(mainShader.program, "al"), 1, &(glm::vec2(this->amb, this->lit))[0]);

	for (int l = 0; l < lightNum; ++l) {
		glUniform3fv(glGetUniformLocation(mainShader.program, ("lights[" + toString(l) + "].pos").c_str()), 1,
					 (const float *) glm::value_ptr(lights[l].pos));
		glUniform3fv(glGetUniformLocation(mainShader.program, ("lights[" + toString(l) + "].col").c_str()), 1,
					 &(lights[l].col * ((float) shadows))[0]);

		GLuint depthTex = glGetUniformLocation(mainShader.program, ("lights[" + toString(l) + "].depthMap").c_str());

		glUniform1i(depthTex, 1 + l);
		glActiveTexture((GLenum) (GL_TEXTURE1 + l));
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l].depthCubemap);
	}

	glUniform1i(glGetUniformLocation(mainShader.program, "lightNum"), lightNum);

	glUniformMatrix4fv(glGetUniformLocation(mainShader.program, "proj"), 1, GL_FALSE, glm::value_ptr(pMatrix));
	glUniformMatrix4fv(glGetUniformLocation(mainShader.program, "view"), 1, GL_FALSE, glm::value_ptr(cMatrix));
	glUniformMatrix4fv(glGetUniformLocation(mainShader.program, "model"), 1, GL_FALSE, glm::value_ptr(uMatrix));
}

void Renderer::flush()
{
	++batchesPerCycle;

	glBindVertexArray(vertexArrayID);

	glBindAttribLocation(mainShader.program, 0, "pos");
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

	glBindAttribLocation(mainShader.program, 1, "norm");
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(
			1,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);

	glBindAttribLocation(mainShader.program, 2, "texture");
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glVertexAttribPointer(
			2,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);

	glBindAttribLocation(mainShader.program, 3, "col");
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
	glVertexAttribPointer(
			3,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glDrawElements(GL_TRIANGLES,
				   bufferIndices, GL_UNSIGNED_INT,
				   nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	bufferIndices = 0;
	bufferVertices = 0;

	uMatrix = glm::mat4(1.0f);
}

void Renderer::render(Texture *tex, glm::mat4 *objMatrix, std::vector<float> *vertexData,
					  std::vector<float> *normalData,
					  std::vector<float> *texData,
					  std::vector<float> *colData, std::vector<int> *indexData)
{
	flushBatch();

	bufferVertices = vertexData->size();
	bufferIndices = indexData->size();
	uMatrix = *objMatrix;
	this->texture = *tex;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(*vertexData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(*texData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(*normalData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(*colData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices * sizeof(int), &(*indexData)[0], GL_DYNAMIC_DRAW);

	flushUpdate();

	flush();
}

void Renderer::drawLine(glm::vec3 l0, glm::vec3 l1)
{
	flushBatch();

	uMatrix = glm::mat4(1.0f);

	std::vector<float> vertexData = {l0.x, l0.y, l0.z, 1.0f, l1.x, l1.y, l1.z, 1.0f};
	std::vector<float> normalData = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::vector<float> texData = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::vector<float> colData = {20.0f, 0.0f, 0.0f, 1.0f, 20.0f, 0.0f, 20.0f, 1.0f};
	std::vector<int> indexData = {0, 1, 0, 1, 0, 1};

	bufferVertices = vertexData.size();
	bufferIndices = indexData.size();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(vertexData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(texData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(normalData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(colData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices * sizeof(int), &(indexData)[0], GL_DYNAMIC_DRAW);

	flushUpdate();

	glBindVertexArray(vertexArrayID);

	glBindAttribLocation(mainShader.program, 0, "pos");
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

	glBindAttribLocation(mainShader.program, 1, "norm");
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(
			1,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);

	glBindAttribLocation(mainShader.program, 2, "texture");
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glVertexAttribPointer(
			2,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);

	glBindAttribLocation(mainShader.program, 3, "col");
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
	glVertexAttribPointer(
			3,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glLineWidth(8.0f);

	glDrawElements(GL_LINES,
				   bufferIndices, GL_UNSIGNED_INT,
				   nullptr);

	glLineWidth(1.0f);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	bufferIndices = 0;
	bufferVertices = 0;

	uMatrix = glm::mat4(1.0f);

	if (!rfbo) renderFBO();
}

void Renderer::flushBatch()
{
	if (bufferIndices > 2) {
		uMatrix = glm::mat4(1.0f);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), vertexArray, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), texArray, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), normalArray, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, colBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), colArray, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices * sizeof(int), indexArray, GL_DYNAMIC_DRAW);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture.tex);

		glm::mat4 oldum = uMatrix;
		int bv = bufferVertices;
		int bi = bufferIndices;

		flush();

		if (drawLines) {
			uMatrix = oldum;
			bufferVertices = bv;
			bufferIndices = bi;

			glBindBuffer(GL_ARRAY_BUFFER, colBuffer);

			for (int i = 0; i < bufferVertices; i += 1)
				colArray[i] = 0.0f;

			glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), colArray, GL_DYNAMIC_DRAW);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnable(GL_POLYGON_OFFSET_LINE);

			glPolygonOffset(0.0f, -500.0f);

			flush();

			glPolygonOffset(0.0f, 0.0f);

			glEnable(GL_POLYGON_OFFSET_LINE);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

void Renderer::drawShadows(std::vector<float> *vertexData, std::vector<int> *indexData)
{
	if (vertexData->size() < BATCH_SIZE && indexData->size() < BATCH_SIZE) {
		addShadows(vertexData, indexData);
	} else {
		renderShadows(&uMatrix, vertexData, indexData);
	}
}

void Renderer::addShadows(std::vector<float> *vertexData, std::vector<int> *indexData)
{
	if ((bufferVertices + vertexData->size() >= BATCH_SIZE ||
		 bufferIndices + indexData->size() >= BATCH_SIZE)) {
		flushShadows();
	}

	int vertNum = bufferVertices / 4;

	for (float &i : *vertexData) {
		vertexArray[bufferVertices] = i;
		++bufferVertices;
	}

	for (int i : *indexData) {
		indexArray[bufferIndices] = i + vertNum;
		++bufferIndices;
	}
}

void Renderer::flushShadows()
{
	uMatrix = glm::mat4(1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), vertexArray, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices * sizeof(int), indexArray, GL_DYNAMIC_DRAW);

	drawShadowMap();
}

void Renderer::renderShadows(glm::mat4 *objMatrix, std::vector<float> *vertexData, std::vector<int> *indexData)
{
	flushShadows();

	bufferVertices = vertexData->size();
	bufferIndices = indexData->size();
	uMatrix = *objMatrix;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferVertices * sizeof(float), &(*vertexData)[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndices * sizeof(int), &(*indexData)[0], GL_DYNAMIC_DRAW);

	drawShadowMap();
}

void Renderer::clearShadows()
{
	for (int l = 0; l < lightNum; ++l) {
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l].depthMapFBO);
		clear();
	}
}

void Renderer::drawShadowMap()
{
	GLfloat aspect = ((GLfloat) SHADOW_WIDTH) / ((GLfloat) SHADOW_HEIGHT);
	GLfloat near = 0.3f;
	GLfloat far = farPlane;
	glm::mat4 shadowProj = glm::perspective((float) (M_PI / 2.0f), aspect, near, far);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glUseProgram(depthShader.program);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	glUniform1f(glGetUniformLocation(depthShader.program, "farPlane"), far);

	glUniformMatrix4fv(glGetUniformLocation(depthShader.program, "model"), 1, GL_FALSE,
					   (const float *) glm::value_ptr(uMatrix));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	for (light_i = 0; light_i < lightNum; ++light_i) {
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(
				shadowProj * glm::lookAt(lights[light_i].pos, lights[light_i].pos + glm::vec3(1.0, 0.0, 0.0),
										 glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(
				shadowProj * glm::lookAt(lights[light_i].pos, lights[light_i].pos + glm::vec3(-1.0, 0.0, 0.0),
										 glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(
				shadowProj * glm::lookAt(lights[light_i].pos, lights[light_i].pos + glm::vec3(0.0, 1.0, 0.0),
										 glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(
				shadowProj * glm::lookAt(lights[light_i].pos, lights[light_i].pos + glm::vec3(0.0, -1.0, 0.0),
										 glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(
				shadowProj * glm::lookAt(lights[light_i].pos, lights[light_i].pos + glm::vec3(0.0, 0.0, 1.0),
										 glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(
				shadowProj * glm::lookAt(lights[light_i].pos, lights[light_i].pos + glm::vec3(0.0, 0.0, -1.0),
										 glm::vec3(0.0, -1.0, 0.0)));

		glBindFramebuffer(GL_FRAMEBUFFER, lights[light_i].depthMapFBO);

		for (GLuint i = 0; i < 6; ++i)
			glUniformMatrix4fv(
					glGetUniformLocation(depthShader.program,
										 ("shadowTransforms[" + toString(i) + "]").c_str()), 1,
					GL_FALSE, glm::value_ptr(shadowTransforms[i])
			);

		glUniform3fv(glGetUniformLocation(depthShader.program, "lightPos"), 1,
					 (const float *) glm::value_ptr(lights[light_i].pos));

		glDrawElements(GL_TRIANGLES,
					   bufferIndices, GL_UNSIGNED_INT,
					   nullptr);
	}

	glDisableVertexAttribArray(0);

	bufferIndices = 0;
	bufferVertices = 0;
}

void Renderer::renderFBO()
{

	float verts[32] = {
			-1.0f, -1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f, -1.0f
	};

	float texes[32] = {
			0.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f, 1.0f,
	};

	int indices[12] = {
			0, 1, 2,
			0, 2, 3
	};

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	clear();

	glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glUseProgram(screenShader.program);

	GLuint stu = glGetUniformLocation(screenShader.program, "screenTex");
	GLuint dtu = glGetUniformLocation(screenShader.program, "ditherTex");

	glUniform1i(stu, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTex);

	glUniform1i(dtu, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ditherTex.tex);

	glUniform1f(glGetUniformLocation(screenShader.program, "dithering"), (float) dithering);
	glUniform2fv(glGetUniformLocation(screenShader.program, "ratio"), 1, &(glm::vec2((float) SCREEN_WIDTH / (float) w, (float) SCREEN_HEIGHT / (float) h))[0]);

	glBindBuffer(GL_ARRAY_BUFFER, quadBuf);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 2 * sizeof(float), verts, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 2 * sizeof(float), texes, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2 * sizeof(int), indices, GL_DYNAMIC_DRAW);

	glBindAttribLocation(screenShader.program, 0, "pos");
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuf);
	glVertexAttribPointer(
			0,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			nullptr
	);
	glBindAttribLocation(screenShader.program, 1, "tex");
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuf);

	glDrawElements(GL_TRIANGLES,
				   12, GL_UNSIGNED_INT,
				   nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	bufferIndices = 0;
	bufferVertices = 0;

	uMatrix = glm::mat4(1.0f);
}

void Renderer::destroyRenderer()
{
	screenShader.destroyShader();
	mainShader.destroyShader();
	depthShader.destroyShader();

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &texBuffer);
	glDeleteBuffers(1, &colBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &quadBuf);
	glDeleteBuffers(1, &indBuf);

	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteTextures(1, &screenTex);
	glDeleteTextures(1, &screenDepthTex);
	glDeleteFramebuffers(1, &screenFBO);
}
