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
	mat = glm::perspective(45.0f, 1.0f, 5.0f, 100.0f);
	mat *= glm::lookAt(
		//glm::vec3(-10,8,0),
		light_pos,
		glm::vec3(0,0,0),
		//glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));
	return mat;
}
void ShadowMap::draw(Camera* cam,std::function<void(Shader* shader)> draw_func)
{
	if(!rt || !shader)return;

	//update light transform
	 // Point toward object regardless of position
	//
	camera_mat = getShadowMatrix();//cam->getProjectionMatrix() * cam->getViewMatrix();
	//program.UpdateUniform("cameraToShadowProjector", mat);

	auto screensize = cam->getScreenSize();
	auto shadowmapsize = getShadowMapSize();
	glViewport(0,0,shadowmapsize.x,shadowmapsize.y);
	shader->begin();
	shader->setUniformMf("cameraToShadowProjector", camera_mat);
	shader->end();

	rt->bind();
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	draw_func(shader);

	rt->bind(false);


	glViewport(0, 0, screensize.x, screensize.y);
}