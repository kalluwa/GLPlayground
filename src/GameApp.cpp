#include "GameApp.h"
#include <fmt/core.h>
#include <pystring.h>

#pragma region construction

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
	m_camera = std::make_unique<Camera>(glm::vec3(10,10,10),
		glm::vec3(0),
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
	for(auto& render_item : m_gameObjects)
	{
		render_item->render(m_camera.get());
	}
}

void GameApp::drawGeometry()
{

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
}

void GameApp::resize(int width,int height)
{
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

	std::unique_ptr<GeometryGenerator> gg(new GeometryGenerator);
	std::string liver_mesh_path = R"(D:\games\organs_models\beihang\liver.obj)";
	GeometryGenerator::MeshData liver_mesh_data;
	if(gg->loadFromFile(liver_mesh_path,liver_mesh_data))
	//gg->createBox(5,10,10,liver_mesh_data);
	//gg->createTriangle(liver_mesh_data);
	{
		//mesh
		std::shared_ptr<RenderItem> liver_object(new RenderItem());
		auto liver_mesh = new MeshGeometry();
		liver_mesh->setBuffer(std::vector<GeometryGenerator::MeshData>{liver_mesh_data});
		liver_object->mesh = liver_mesh;
		liver_mesh->name = "liver";
		//TODO:Material
		
		//shader
		std::unique_ptr<Shader> basic_shader(new Shader);
		basic_shader->setFromFile(R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\basic.vs)",R"(C:\Users\kallu\source\repos\LiverViewer\resources\shaders\basic.fs)");
		this->m_shaders["basic"] = std::move(basic_shader);
		liver_object->shader = m_shaders["basic"].get();



		//push all
		m_gameObjects.emplace_back(liver_object);

	};

}


#pragma endregion



