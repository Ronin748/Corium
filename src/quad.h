//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_QUAD_H
#define CORIUM_QUAD_H

#include "mesh.h"
#include "texture.h"
#include "util/quadIsect.h"

class Quad : public Mesh {
public:
	Texture tex;
	Phys phys;

	void update()
	{
		objMatrix = phys.getMatrix();
		vertexData = transform(vertexData0, objMatrix);
		getNormals();
		getBoundingSphere();
	}

	void draw(Renderer *renderer)
	{
		renderer->draw(&tex, &vertexData, &normalData, &texData, &colData, &indexData);
	}

	void loadSprite(std::string tex_path, glm::vec4 tex_sprite, bool load_tex = true)
	{
		if (load_tex) {
			tex.loadTexture(tex_path);
		}
		float s_offs = (tex_sprite.s / (float) tex.w) * (tex.w / tex_sprite.p);
		float t_offs = (tex_sprite.t / (float) tex.h) * (tex.h / tex_sprite.q);

		float p_scale = tex_sprite.p / (float) tex.w;
		float q_scale = tex_sprite.q / (float) tex.h;

		texData = {
				0.0f + s_offs, 0.0f + t_offs,
				p_scale, q_scale,
				1.0f + s_offs, 0.0f + t_offs,
				p_scale, q_scale,
				1.0f + s_offs, 1.0f + t_offs,
				p_scale, q_scale,
				0.0f + s_offs, 1.0f + t_offs,
				p_scale, q_scale
		};
	}

	Quad() : Mesh()
	{
		vertexData0.resize(16);
		texData.resize(16);
		colData.resize(16);
		indexData.resize(6);
		normalData.resize(6);

		vertexData0 = {
				0.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f
		};
		vertexData = {
				0.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f
		};

		indexData = {
				0, 1, 2,
				0, 2, 3
		};

		texData = {
				0.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				0.0f, 1.0f, 1.0f, 1.0f
		};
		colData = {
				1.0, 1.0, 1.0, 1.0,
				1.0, 1.0, 1.0, 1.0,
				1.0, 1.0, 1.0, 1.0,
				1.0, 1.0, 1.0, 1.0
		};
		normalData = {
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f
		};
		getNormals();

		phys.update();
	}
};


#endif //CORIUM_QUAD_H
