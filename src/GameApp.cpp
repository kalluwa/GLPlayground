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
	m_camera = std::make_unique<Camera>(glm::vec3(-10.896174,7.108711,-38.775303),
		glm::vec3(-0.8545703,-4.8266044,-12.192807),
		glm::vec3(0,1,0),
		45.0f,
		m_viewport.x/(float)(m_viewport.y),
		0.1f,
		5000.0f);

	
	loadContent();

	return true;
}

void GameApp::update(float delta_time)
{
	m_camera->update();
}

void GameApp::draw()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);

	drawGBuffer();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	for(auto& render_item : m_gameObjects)
	{
		//set basic shader
		render_item->shader = m_shaders["basic"].get();
		render_item->render(m_camera.get());
	}

	//draw debugviewer
	this->m_debug_viewer->drawTex(g_tex_id,0,0,100,100);
}

void GameApp::drawGBuffer()
{
	if(!m_gbuffer)
		return;

	if(!m_gbuffer->shader)
		m_gbuffer->shader = m_shaders["gbuffer"].get();

	m_gbuffer->draw([&](Shader* shader){
		for (auto& render_item : m_gameObjects)
		{
			//set basic shader
			render_item->shader = shader;
			render_item->render(m_camera.get());
		}
	});
}
#pragma endregion


#pragma region input
void GameApp::inputMouseScroll(int x)
{
	m_camera->updateMouseDelta(x);
}

void GameApp::inputMouse(int x, int y, int mouse_button, int mouse_press_or_release)
{
	m_camera->updateMouseState(x,y,mouse_button, mouse_press_or_release);

	//auto pos = m_camera->getPosition();
	//auto target = m_camera->getTarget();
	//fmt::print("cam pos:{},{},{}\n cam target:{},{},{}\n",pos.x,pos.y,pos.z,target.x,target.y,target.z);
}

void GameApp::resize(int width,int height)
{
	m_viewport = glm::ivec2(width,height);
	m_camera->viewport(0,0,width,height);
	glViewport(0,0,width,height);
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
	basic_shader->setFromFile(R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\basic.vs)", 
		R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\basic.fs)");
	this->m_shaders["basic"] = std::move(basic_shader);

	//gbuffer
	std::unique_ptr<Shader> gbuffer_shader(new Shader);
	gbuffer_shader->setFromFile(R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\gbuffer.vs)", 
		R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\gbuffer.fs)");
	this->m_shaders["gbuffer"] = std::move(gbuffer_shader);

	std::unique_ptr<Shader> screenquad_shader(new Shader);
	screenquad_shader->setFromFile(R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\screenquad.vs)", 
		R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\screenquad.fs)");
	this->m_shaders["screenquad"] = std::move(screenquad_shader);

	//-------------shader end
	// 
	//gbuffer
	m_gbuffer = std::make_unique<GBuffer>();
	m_gbuffer->build(m_viewport.x,m_viewport.y);


	//load mesh
	std::unique_ptr<GeometryGenerator> gg(new GeometryGenerator);
	std::string liver_mesh_path = R"(D:\games\organs_models\beihang\liver.obj)";
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
		//liver_object->shader = m_shaders["basic"].get();

		//push all
		m_gameObjects.emplace_back(liver_object);

	};


	m_debug_viewer = std::make_unique<DebugViewer>(this);
	m_debug_viewer->shader = this->m_shaders["screenquad"].get();

	//load test image texture
	auto loader = new TextureLoader();
	auto tex = loader->load(R"(D:\projects\DX11-Car-Demo-master\Project1\Tex\floor.dds)");
	g_tex_id = tex->tex_id;
	fmt::print("tex id : {}\n",tex->tex_id);

}


#pragma endregion



