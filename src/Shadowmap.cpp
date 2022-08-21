#include "Shadowmap.h"
#include "GameApp.h"

ShadowMap::ShadowMap(GameApp* app)
{
	this->app = app;
	//rt = new RenderTarget(width,height,{GL_FLOAT});

	intCSM();
}

ShadowMap::~ShadowMap()
{

}

void ShadowMap::resize(int w,int h)
{
	//rt->change(w,h);
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
	//if(!rt || !shader)return;

	////update light transform
	// // Point toward object regardless of position
	////
	//camera_mat = getShadowMatrix();//cam->getProjectionMatrix() * cam->getViewMatrix();
	////program.UpdateUniform("cameraToShadowProjector", mat);

	//auto screensize = cam->getScreenSize();
	//auto shadowmapsize = getShadowMapSize();
	//glViewport(0,0,shadowmapsize.x,shadowmapsize.y);
	//shader->begin();
	//shader->setUniformMf("cameraToShadowProjector", camera_mat);
	//shader->end();

	//rt->bind();
	//glClearColor(1,1,1,1);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//draw_func(shader);

	//rt->bind(false);


	//glViewport(0, 0, screensize.x, screensize.y);
}

void ShadowMap::intCSM()
{
	// build and compile shaders
	// -------------------------
	std::unique_ptr<Shader> shader(new Shader);
	std::unique_ptr<Shader> simpleDepthShader(new Shader);//,debugDepthQuad,debugCascadeShader;
	shader->setFromFile("resources/shaders/csm/10.shadow_mapping.vs", "resources/shaders/csm/10.shadow_mapping.fs");
	simpleDepthShader->setFromFile("resources/shaders/csm/10.shadow_mapping_depth.vs", "resources/shaders/csm/10.shadow_mapping_depth.fs", "resources/shaders/csm/10.shadow_mapping_depth.gs");
	//debugDepthQuad->setFromFile("10.debug_quad.vs", "10.debug_quad_depth.fs");
	//debugCascadeShader->setFromFile("10.debug_cascade.vs", "10.debug_cascade.fs");

	app->getShaders()["ShadowMapping"] = std::move(shader);
	app->getShaders()["ShadowDepth"] = std::move(simpleDepthShader);

}