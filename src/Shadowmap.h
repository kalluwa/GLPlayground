#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include "RenderStruct.h"
#include "camera.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>

class GameApp;

class ShadowMap
{
public:
	ShadowMap(GameApp* app);
	~ShadowMap();

	void resize(int w,int h);
	void draw(Camera* cam,std::function<void(Shader*)> draw_func);

	glm::mat4 getShadowMatrix();

	glm::ivec2 getShadowMapSize()
	{
		if(!rt) return glm::ivec2 {512,512};

		return rt->getSize();
	};

	Shader* shader = nullptr;
	RenderTarget* rt = nullptr;

	glm::vec3 light_pos = glm::vec3(5,5,5);
	

	glm::mat4 camera_mat = glm::mat4(1.0f);
	

	//csm functions
	void intCSM();




	// settings
	const unsigned int SCR_WIDTH = 2560;
	const unsigned int SCR_HEIGHT = 1440;

	const float cameraNearPlane = 0.1f;
	const float cameraFarPlane = 500.0f;

	std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };



	// lighting info
	// -------------
	const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
	unsigned int lightFBO;
	unsigned int lightDepthMaps;
	const unsigned int depthMapResolution = 4096;

	//Ëæ»úÊý£º·ÅÖÃcube
	std::random_device device;
	std::mt19937 generator = std::mt19937(device());

	std::vector<glm::mat4> lightMatricesCache;

	GameApp* app = nullptr;
};
#endif