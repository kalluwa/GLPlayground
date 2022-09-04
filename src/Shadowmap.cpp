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

	SCR_WIDTH = w;
	SCR_HEIGHT = h;
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
void ShadowMap::draw(FPSCamera* cam,std::function<void(Shader* shader)> draw_func)
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
	std::unique_ptr<Shader> shaderUnique(new Shader);
	std::unique_ptr<Shader> simpleDepthShaderUnique(new Shader);//,debugDepthQuad,debugCascadeShader;
	shaderUnique->setFromFile("resources/shaders/csm/10.shadow_mapping.vs", "resources/shaders/csm/10.shadow_mapping.fs");
	simpleDepthShaderUnique->setFromFile("resources/shaders/csm/10.shadow_mapping_depth.vs", "resources/shaders/csm/10.shadow_mapping_depth.fs", "resources/shaders/csm/10.shadow_mapping_depth.gs");
	//debugDepthQuad->setFromFile("10.debug_quad.vs", "10.debug_quad_depth.fs");
	//debugCascadeShader->setFromFile("10.debug_cascade.vs", "10.debug_cascade.fs");

	app->getShaders()["ShadowMapping"] = std::move(shaderUnique);
	app->getShaders()["ShadowDepth"] = std::move(simpleDepthShaderUnique);


	// configure light FBO
	// -----------------------
	glGenFramebuffers(1, &lightFBO);

	glGenTextures(1, &lightDepthMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(shadowCascadeLevels.size()) + 1,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
		throw 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//uniform buffer
	// configure UBO
	// --------------------
	glGenBuffers(1, &matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	Shader* shader = app->getShaders()["ShadowMapping"].get();
	shader->begin();
	shader->setUniformInt("diffuseTexture", 0);
	shader->setUniformInt("shadowMap", 1);


}

#pragma region csm

//[8 corners]frustum -> world space
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
{
	const auto inv = glm::inverse(projview);

	std::vector<glm::vec4> frustumCorners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}
//same to getFrustumCornersWorldSpace(mat)
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
	return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 getLightSpaceMatrix(FPSCamera* camera,const float nearPlane, const float farPlane)
{
	auto size = camera->getScreenSize();

	const auto proj = glm::perspective(
		glm::radians(camera->Zoom), (float)size.x / (float)size.y, nearPlane,
		farPlane);
	const auto corners = getFrustumCornersWorldSpace(proj, camera->getViewMatrix());

	glm::vec3 center = glm::vec3(0, 0, 0);
	for (const auto& v : corners)
	{
		center += glm::vec3(v);
	}
	center /= corners.size();

	const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
	const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();
	for (const auto& v : corners)
	{
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	// Tune this parameter according to the scene
	constexpr float zMult = 10.0f;
	if (minZ < 0)
	{
		minZ *= zMult;
	}
	else
	{
		minZ /= zMult;
	}
	if (maxZ < 0)
	{
		maxZ /= zMult;
	}
	else
	{
		maxZ *= zMult;
	}

	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

std::vector<glm::mat4> ShadowMap::getLightSpaceMatrices()
{
	std::vector<glm::mat4> ret;
	for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
	{
		if (i == 0)
		{
			ret.push_back(getLightSpaceMatrix(app->getCamera(),cameraNearPlane, shadowCascadeLevels[i]));
		}
		else if (i < shadowCascadeLevels.size())
		{
			ret.push_back(getLightSpaceMatrix(app->getCamera(),shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
		}
		else
		{
			ret.push_back(getLightSpaceMatrix(app->getCamera(),shadowCascadeLevels[i - 1], cameraFarPlane));
		}
	}
	return ret;
}

#pragma endregion

void ShadowMap::drawCSM(FPSCamera* cam,std::function<void(Shader* shader)> draw_func)
{
	//…Ë÷√uniform buffer
	//setup ubo
	// 0. UBO setup
	const auto lightMatrices = getLightSpaceMatrices();
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	for (size_t i = 0; i < lightMatrices.size(); ++i)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	//ªÊ÷∆depth
	{
		auto simpleDepth = app->getShaders()["ShadowDepth"].get();
		simpleDepth->begin();

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glViewport(0, 0, depthMapResolution, depthMapResolution);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);  // peter panning
		
		draw_func(simpleDepth);

		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		simpleDepth->end();
	}


	// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Shader* shader = app->getShaders()["ShadowMapping"].get();

	//shader->begin();
	//const glm::mat4 projection = cam->getProjectionMatrix();//glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, cameraNearPlane, cameraFarPlane);
	//const glm::mat4 view = cam->getViewMatrix();
	//shader->setUniformMf("projection", projection);
	//shader->setUniformMf("view", view);
	//// set light uniforms
	//shader->setUniformVf("viewPos", cam->Position);
	//shader->setUniformVf("lightDir", lightDir);
	//shader->setUniformF("farPlane", cameraFarPlane);
	//shader->setUniformInt("cascadeCount", shadowCascadeLevels.size());
	//for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
	//{
	//	shader->setUniformF("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
	//}
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
	//draw_func(shader);

	//shader->end();
}