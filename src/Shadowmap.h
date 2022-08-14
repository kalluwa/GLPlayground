#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include "RenderStruct.h"
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

class ShadowMap
{
public:
	ShadowMap(int width=1024,int height=1024);
	~ShadowMap();

	void resize(int w,int h);
	void draw(Camera* cam,std::function<void(Shader*)> draw_func);

	glm::mat4 getShadowMatrix();

	Shader* shader = nullptr;
	RenderTarget* rt = nullptr;

	glm::vec3 light_pos = glm::vec3(10,10,10);
	

	glm::mat4 camera_mat = glm::mat4(1.0f);
};
#endif