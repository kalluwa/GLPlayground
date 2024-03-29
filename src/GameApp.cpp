#include "GameApp.h"
#include <fmt/core.h>
#include <pystring.h>
#include <TextureLoader.h>

#pragma region construction

unsigned int g_tex_id = 0;

GameApp::GameApp(int w,int h)
	:m_viewport(w,h)
{
}
GameApp::~GameApp()
{
}

#pragma endregion

#pragma region main function

bool GameApp::init()
{
	m_camera = std::make_unique<Camera>(
		glm::vec3(0,4,5),
		glm::vec3(0,0,0),
		glm::vec3(0,1,0),
		45.0f,
		m_viewport.x/(float)(m_viewport.y),
		0.1f,
		1000.0f);

	m_fpscamera = std::make_unique<FPSCamera>(glm::vec3(0.0f, 0.0f, 3.0f)
		);
	
	loadContent();

	return true;
}

void GameApp::update(float delta_time)
{
	m_camera->update();

}

void GameApp::draw()
{
	

	drawGBuffer();

	drawShadowMap();

	//glClearColor(1,1,1,1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//for(auto& render_item : m_gameObjects)
	//{
	//	//set basic shader
	//	render_item->shader = m_shaders["basic"].get();
	//	render_item->render(m_camera.get());
	//}
	// 
	auto shadowmap_tex = m_shadow_map->getShadowMaps();
	auto& rts = m_gbuffer->render_target->getRenderTarget();
	//auto& rt_shadow = m_shadow_map->rt->getRenderTarget();
	//post process
	if(m_post_composer && m_post_composer->m_passes.size())
	{
		//apply gbuffer to all pass
		m_post_composer->setGBuffer(rts[0],rts[1],rts[2]);

		//set first pass
		auto& thispass = m_post_composer->m_passes[0];
		thispass->textures["position_map"] = TextureUint(rts[0]);
		thispass->textures["normal_map"] = TextureUint(rts[1]);
		thispass->textures["diffuse_map"] = TextureUint(rts[2]);
		thispass->textures["noise_map"] = TextureUint(this->m_textures["noise_tex"]->tex_id);
		thispass->textures["shadowMap"] = TextureUint(shadowmap_tex,GL_TEXTURE_2D_ARRAY);

		thispass->shader = m_shaders["lighting_default"].get();
		//set shadow matrix
		thispass->shader->begin();
		thispass->shader->setUniformMf("cameraToShadowProjector",m_shadow_map->getShadowMatrix());
		
		//csm
		const glm::mat4 projection = m_fpscamera->getProjectionMatrix();//glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, cameraNearPlane, cameraFarPlane);
		const glm::mat4 view = m_fpscamera->getViewMatrix();
		thispass->shader->setUniformMf("projection", projection);
		thispass->shader->setUniformMf("view", view);
		// set light uniforms
		thispass->shader->setUniformVf("viewPos", m_fpscamera->Position);
		thispass->shader->setUniformVf("lightDir", m_shadow_map->lightDir);
		thispass->shader->setUniformF("farPlane", m_shadow_map->cameraFarPlane);
		thispass->shader->setUniformInt("cascadeCount", m_shadow_map->shadowCascadeLevels.size());
		for (size_t i = 0; i < m_shadow_map->shadowCascadeLevels.size(); ++i)
		{
			thispass->shader->setUniformF("cascadePlaneDistances[" + std::to_string(i) + "]", m_shadow_map->shadowCascadeLevels[i]);
		}

		thispass->shader->end();

		m_post_composer->draw();
	}

	//buffer debug
	/*if(aopass)
	{
		aopass->pos_map = rts[0];
		aopass->normal_map = rts[1];
		aopass->color_map = rts[2];

		aopass->draw(getCamera());
	}*/

	//draw debugviewer
	//auto viewsize = getCamera()->getScreenSize();
	//this->m_debug_viewer->drawTex(rts[0], 0, viewsize.y * 0.75f, viewsize.x * 0.25f, viewsize.y * 0.25f);
	//this->m_debug_viewer->drawTex(rts[1], viewsize.x * 0.25f, viewsize.y * 0.75f, viewsize.x * 0.25f, viewsize.y * 0.25f);
	//this->m_debug_viewer->drawTex(rts[2], viewsize.x * 0.5f, viewsize.y * 0.75f, viewsize.x * 0.25f, viewsize.y * 0.25f);
	//this->m_debug_viewer->drawTex(g_tex_id, 500, 500, 400, 400);

	//this->m_debug_viewer->drawTex(rt_shadow[0],viewsize.x * 0.75f, viewsize.y * 0.75f, viewsize.x * 0.25f, viewsize.y * 0.25f);
}

void GameApp::drawGBuffer()
{
	if(!m_gbuffer)
		return;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!m_gbuffer->shader)
		m_gbuffer->shader = m_shaders["gbuffer"].get();

	m_gbuffer->draw([&](Shader* shader){
		for (auto& render_item : m_gameObjects)
		{
			//set basic shader
			render_item->shader = shader;
			render_item->render(getCamera());
		}
	});
}

void GameApp::drawShadowMap()
{
	if(!m_shadow_map)
		return;

	//if (!m_shadow_map->shader)
	//	m_shadow_map->shader = m_shaders["shadowmap"].get();

	//m_shadow_map->draw(getCamera(),[&](Shader* shader){
	//	for (auto& render_item : m_gameObjects)
	//	{
	//		//set basic shader
	//		render_item->shader = shader;
	//		render_item->render(nullptr);
	//	}
	//});

	m_shadow_map->drawCSM(getCamera(), [&](Shader* shader) {
		for (auto& renderItem : m_gameObjects)
		{
			//set basic shader
			renderItem->shader = shader;
			renderItem->render(nullptr);
		}
	});
}
#pragma endregion


#pragma region input
void GameApp::inputMouseScroll(int x)
{
	m_camera->updateMouseDelta(x);

	m_fpscamera->ProcessMouseScroll(x);
}

void GameApp::inputMouse(int x, int y, int mouse_button, int mouse_press_or_release)
{
	m_camera->updateMouseState(x,y,mouse_button, mouse_press_or_release);

	//auto pos = m_camera->getPosition();
	//auto target = m_camera->getTarget();
	//fmt::print("cam pos:{},{},{}\n cam target:{},{},{}\n",pos.x,pos.y,pos.z,target.x,target.y,target.z);

	static bool firsthit = true;
	static int lastx = 0;
	static int lasty = 0;
	int deltax = 0,deltay = 0;

	if(mouse_button==0 && mouse_press_or_release)
	{
		firsthit = true;
		lastx = 0;
		lasty = 0;
	}

	if(firsthit)
	{
		lastx = x;
		lasty = y;
		firsthit = false;
	}
	else
	{
		deltax = x - lastx;
		deltay = lasty - y;
		lastx = x;
		lasty = y;
	}
	m_fpscamera->ProcessMouseMovement(deltax,deltay);
}

void GameApp::resize(int width,int height)
{
	m_viewport = glm::ivec2(width,height);
	m_camera->viewport(0,0,width,height);
	glViewport(0,0,width,height);

	m_fpscamera->viewport(0, 0, width, height);
	//�¼��䶯
	m_gbuffer->render_target->change(width,height);
}
void GameApp::inputKeyboard(int keycode, int action)
{
	static bool key_shift_ = false;
	static bool key_ctrl_ = false;

	if (keycode == -1)//shift
	{
		key_shift_ = action > 0;
	}
	if (keycode == -2) //ctrl
	{
		key_ctrl_ = action > 0;
	}

	if (key_ctrl_ && keycode > 0 && action > 0) //press down
	{
		if (keycode == 'c' || keycode == 'C')
		{
		}
	}

	getCamera()->MovementSpeed = key_shift_ ? 2.5 * 10 : 2.5;
	const float deltaTime = 0.1f;
	if(action > 0)
	{
		if (keycode == 'W') getCamera()->ProcessKeyboard(FORWARD, deltaTime);
		if (keycode == 'S') getCamera()->ProcessKeyboard(BACKWARD, deltaTime);
		if (keycode == 'A') getCamera()->ProcessKeyboard(LEFT, deltaTime);
		if (keycode == 'D') getCamera()->ProcessKeyboard(RIGHT, deltaTime);
	}
}
#pragma endregion

#pragma region protected functions

void GameApp::loadContent()
{
	fmt::print("load content here\n");

	//load shader
	//-------------shader
	
		//basic
	std::unique_ptr<Shader> basic_shader(new Shader);
	basic_shader->setFromFile(R"(resources\shaders\basic.vs)", 
		R"(resources\shaders\basic.fs)");
	this->m_shaders["basic"] = std::move(basic_shader);

	//gbuffer
	std::unique_ptr<Shader> gbuffer_shader(new Shader);
	gbuffer_shader->setFromFile(R"(resources\shaders\gbuffer.vs)", 
		R"(resources\shaders\gbuffer.fs)");
	this->m_shaders["gbuffer"] = std::move(gbuffer_shader);

	//lighting
	std::unique_ptr<Shader> lighting_default_shader(new Shader);
	lighting_default_shader->setFromFile(R"(resources\shaders\lighting_default.vs)",
		R"(resources\shaders\lighting_default.fs)");
	this->m_shaders["lighting_default"] = std::move(lighting_default_shader);

	//fxaa
	std::unique_ptr<Shader> fxaa_shader(new Shader);
	fxaa_shader->setFromFile(R"(resources\shaders\fxaa.vs)",
		R"(resources\shaders\fxaa.fs)");
	this->m_shaders["fxaa"] = std::move(fxaa_shader);

	//shadowmap
	std::unique_ptr<Shader> shadowmap_shader(new Shader);
	shadowmap_shader->setFromFile(R"(resources\shaders\shadowmap.vs)",
		R"(resources\shaders\shadowmap.fs)");
	this->m_shaders["shadowmap"] = std::move(shadowmap_shader);

	//debug viewer
	std::unique_ptr<Shader> screenquad_shader(new Shader);
	screenquad_shader->setFromFile(R"(resources\shaders\screenquad.vs)", 
		R"(resources\shaders\screenquad.fs)");
	this->m_shaders["screenquad"] = std::move(screenquad_shader);

	//-------------shader end
	// 
	//gbuffer
	m_gbuffer = std::make_unique<GBuffer>();
	m_gbuffer->build(m_viewport.x,m_viewport.y);

	//shadowmap
	m_shadow_map = std::make_unique<ShadowMap>(this);
	//m_shadow_map->shader = m_shaders["shadermap"].get();
	// 
	//ssao
	

	//textures-----------------
	//load basictexture
	auto loader = new TextureLoader();
	auto liver_model_tex = loader->load(R"(resources\models\liver\FQJ_DanNang_Gan.png)");
	m_textures["liver_diffuse"] = std::move(liver_model_tex);

	//random texture
	auto noise_tex = loader->genTex(256,256,3);
	m_textures["noise_tex"] = std::move(noise_tex);
	// textures----------------------
	//load mesh
	std::unique_ptr<GeometryGenerator> gg(new GeometryGenerator);
	std::string liver_mesh_path = R"(resources\models\liver\liver.obj)";
		//R"(resources\models\cube\cube.obj)";
	GeometryGenerator::MeshData liver_mesh_data;
	if(gg->loadFromFile(liver_mesh_path,liver_mesh_data))
	{
		//gg->createBox(5,10,10,liver_mesh_data);
		//gg->createTriangle(liver_mesh_data);
		//mesh
		std::shared_ptr<RenderItem> liver_object(new RenderItem());
		auto liver_mesh = new MeshGeometry();
		liver_mesh->setBuffer(std::vector<GeometryGenerator::MeshData>{liver_mesh_data});
		liver_object->mesh = liver_mesh;
		liver_mesh->name = "liver";
		liver_object->tex_diffuse = m_textures["liver_diffuse"].get();

		//push all
		m_gameObjects.emplace_back(liver_object);

	};
	//load ground plane
	GeometryGenerator::MeshData plane_data;
	gg->createGrid(100,100,2,2,plane_data);
	std::shared_ptr<RenderItem> plane_object(new RenderItem());
	auto plane_mesh = new MeshGeometry();
	plane_mesh->setBuffer(std::vector<GeometryGenerator::MeshData>{plane_data});
	
	plane_mesh->name = "groundplane";
	plane_object->mesh = plane_mesh;
	plane_object->transform.position = glm::vec3(0,-9,0);
	plane_object->transform.update();

	m_gameObjects.emplace_back(plane_object);
	//cube
	{
		GeometryGenerator::MeshData crawl_mesh_data;
		if (gg->loadFromFile(R"(resources\models\cube\crawl.obj)", crawl_mesh_data))
		{
			std::shared_ptr<RenderItem> cube_object(new RenderItem());
			auto cube_mesh = new MeshGeometry();
			cube_mesh->setBuffer(std::vector<GeometryGenerator::MeshData>{crawl_mesh_data});

			cube_mesh->name = "crawl";
			cube_object->mesh = cube_mesh;
			cube_object->transform.position = glm::vec3(0, -6, 0);
			cube_object->transform.update();

			cube_object->mat->diffuse = glm::vec3(1.0,0.9,0.1);
			m_gameObjects.emplace_back(cube_object);
		}
		
	}

	{
		//lightbox
		{
			GeometryGenerator::MeshData cube_data;
			gg->createBox(1, 1, 1, cube_data);
			std::shared_ptr<RenderItem> cube_object(new RenderItem());
			auto cube_mesh = new MeshGeometry();
			cube_mesh->setBuffer(std::vector<GeometryGenerator::MeshData>{cube_data});

			cube_mesh->name = "cube";
			cube_object->mesh = cube_mesh;
			cube_object->transform.position = m_shadow_map->light_pos;//glm::vec3(-10, 8, 0);
			cube_object->transform.update();
			m_gameObjects.emplace_back(cube_object);
		}
	}
	//
	


	//lighting merge item
	m_post_composer = std::make_unique<PostComposer>(this);
	std::unique_ptr<PostPass> lightpass(new PostPass(this->m_shaders["lighting_default"].get()));
	//m_post_composer->m_passes.emplace_back(lightpass);
	m_post_composer->add(std::move(lightpass));

	//ssao pass
	std::unique_ptr<AOPass> aopass(new AOPass(this));
	m_post_composer->add(std::move(aopass));
	//aopass = new AOPass(this);

	//aa pass
	std::unique_ptr<PostPass> aapass(new FXAAPass(this->m_shaders["fxaa"].get()));
	m_post_composer->add(std::move(aapass));

	
	m_debug_viewer = std::make_unique<DebugViewer>(this);
	m_debug_viewer->shader = this->m_shaders["screenquad"].get();

	//load test image texture
	//auto loader = new TextureLoader();
	//auto tex = loader->load(R"(C:\Program Files (x86)\Epic Games\Launcher\Portal\SysFiles\0619_Rogue_Company.png)");
	//g_tex_id = tex->tex_id;
	//fmt::print("tex id : {}\n",tex->tex_id);

}


#pragma endregion



