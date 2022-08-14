#include "Shadowmap.h"

ShadowMap::ShadowMap(int width,int height)
{
	rt = new RenderTarget(width,height,{GL_FLOAT});

	
}

ShadowMap::~ShadowMap()
{

}

void ShadowMap::resize(int w,int h)
{
	rt->change(w,h);
}

glm::mat4 ShadowMap::getShadowMatrix()
{
	glm::mat4 mat;
	mat = glm::perspective(45.0f, 1.0f, 0.1f, 50.0f);
	mat *= glm::lookAt(
		light_pos,
		glm::vec3(0, 0, 0)
		, glm::vec3(0, 1, 0));
	return mat;
}
void ShadowMap::draw(Camera* cam,std::function<void(Shader* shader)> draw_func)
{
	if(!rt || !shader)return;

	//update light transform
	 // Point toward object regardless of position
	camera_mat = getShadowMatrix();//cam->getProjectionMatrix() * cam->getViewMatrix();
	//program.UpdateUniform("cameraToShadowProjector", mat);

	shader->begin();
	shader->setUniformMf("cameraToShadowProjector", camera_mat);
	shader->end();

	rt->bind();

	draw_func(shader);

	rt->bind(false);
}