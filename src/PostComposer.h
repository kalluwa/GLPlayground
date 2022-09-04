#ifndef POST_COMPOSER_H_
#define POST_COMPOSER_H_

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include<fmt/core.h>
#include "RenderStruct.h"
#include "fpscamera/fpscamera.h"

class GameApp;
struct TextureUint
{
	unsigned int tex_id=0;
	unsigned int tex_type = GL_TEXTURE_2D;//GL_TEXTURE_2D_ARRAY

	TextureUint(unsigned int id=0)
	{
		tex_id = id;
	}
	TextureUint(unsigned int id,unsigned int ttype)
	{
		tex_id = id;
		tex_type = ttype;
	}
};

class PostPass
{
public:
	PostPass(Shader* shader=nullptr)
	{
		this->shader = shader;
		this->textures.clear();

		//auto rf = [](){ return rand() / (float)RAND_MAX;};
		//for (int i = 0; i < 64; ++i)
		//{
		//	glm::vec3 randvec;
		//	randvec[0] = rf() * 2.0f - 1.0f;
		//	randvec[1] = rf() * 2.0f - 1.0f;
		//	randvec[2] = rf();
		//	randvec = rf() * glm::normalize(randvec);

		//	float scale = float(i) / 64.0f;
		//	scale = glm::lerp(0.1f, 1.0f, scale);
		//	randvec *= scale;
		//	offset_positions[i] = randvec;
		//}
	}
	
	virtual void draw(FPSCamera* cam);

	int passthrough = 1;
	std::string tagname = "";
	Shader* shader = nullptr;
	std::map<std::string,TextureUint> textures;
	//glm::vec3 offset_positions[64];

	//gbuffer map
	unsigned int pos_map,normal_map,color_map;
};

class AOPass : public PostPass
{
public:
	AOPass(GameApp* app);

	void draw(FPSCamera* cam) override;

	std::vector<glm::vec3> ssaoKernel;
	Shader* blur_shader = nullptr;
	unsigned int noiseTexture; 
};

class LightPass : public PostPass
{
public:
	virtual void draw(FPSCamera* cam) override
	{
		PostPass::draw(cam);
	}
};

class FXAAPass : public PostPass
{
public:
	FXAAPass(Shader* shader=nullptr)
	{
		this->shader = shader;
		this->textures.clear();
	}

	void draw(FPSCamera* cam) override
	{
		if(this->shader)
		{
			auto invsize = 1.0f / cam->getScreenSize();
			shader->begin();
			shader->setUniformVf("resolution", invsize);
			shader->end();

		}
		PostPass::draw(cam);
	}

};

class PostComposer
{
public:
	PostComposer(GameApp* app);

	~PostComposer();

	void draw();

	void add(std::unique_ptr<PostPass>&& pass);

	void setGBuffer(unsigned int pos,unsigned int normal,unsigned int color)
	{
		for(auto& pass : m_passes)
		{
			pass->color_map = color;
			pass->normal_map = normal;
			pass->pos_map = pos;
		}
	}
	//ÃÌº”‰÷»æµƒpass
	std::vector<std::unique_ptr<PostPass>> m_passes;
	GameApp* app = nullptr;

	std::vector<RenderTarget*> rts;
};
#endif