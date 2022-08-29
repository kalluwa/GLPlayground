#ifndef POST_COMPOSER_H_
#define POST_COMPOSER_H_

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include<fmt/core.h>
#include "RenderStruct.h"
#include "fpscamera/fpscamera.h"

class PostPass
{
public:
	PostPass(Shader* shader=nullptr)
	{
		this->shader = shader;
		this->textures.clear();

		auto rf = [](){ return rand() / (float)RAND_MAX;};
		for (int i = 0; i < 64; ++i)
		{
			glm::vec3 randvec;
			randvec[0] = rf() * 2.0f - 1.0f;
			randvec[1] = rf() * 2.0f - 1.0f;
			randvec[2] = rf();
			randvec = rf() * glm::normalize(randvec);

			float scale = float(i) / 64.0f;
			scale = glm::lerp(0.1f, 1.0f, scale);
			randvec *= scale;
			offset_positions[i] = randvec;
		}
	}
	
	virtual void draw(FPSCamera* cam);

	Shader* shader = nullptr;
	std::map<std::string,unsigned int> textures;
	glm::vec3 offset_positions[64];
};

class LightPass : public PostPass
{
public:
	virtual void draw(FPSCamera* cam) override
	{
		PostPass::draw(cam);
	}
};
class GameApp;
class PostComposer
{
public:
	PostComposer(GameApp* app);

	~PostComposer();

	void draw();

	void add(std::unique_ptr<PostPass>&& pass)
	{
		m_passes.emplace_back(std::move(pass));
	}

	//ÃÌº”‰÷»æµƒpass
	std::vector<std::unique_ptr<PostPass>> m_passes;
	GameApp* app = nullptr;

	RenderTarget* rt=nullptr;
};
#endif