#include <unordered_map>
#include <string>
#include <iostream>

#include <RenderItem.h>
#include <camera.h>
#include <GBuffer.h>
#include <debugViewer.h>

class GameApp
{
public:
	GameApp(int w=800,int h=600);
	~GameApp();

	//主要方法
	//初始化
	virtual bool init();
	//更新
	virtual void update(float delta_time);
	//绘制
	virtual void draw();

	virtual void drawGBuffer();

	//!input mouse
	void inputMouse(int x, int y, int mouse_button, int mouse_press_or_release);

	void inputMouseScroll(int x);

	//!input keyboard
	void inputKeyboard(int keycode, int action);

	Camera* getCamera(){ return m_camera.get();}

	void resize(int width,int height);
protected:
	//读取初始的模型数据 纹理数据
	virtual void loadContent();

protected:
	//存储所有数据

	//几何
	std::unordered_map<std::string,std::unique_ptr<MeshGeometry>> m_geometries;
	//材质
	std::unordered_map<std::string,std::unique_ptr<Material>> m_materials;
	//纹理
	std::unordered_map<std::string,std::unique_ptr<Texture>> m_textures;
	//shader
	std::unordered_map<std::string,std::unique_ptr<Shader>> m_shaders;

	glm::ivec2 m_viewport;
	std::unique_ptr<Camera> m_camera = nullptr;

	//三维对象
	std::vector<std::shared_ptr<RenderItem>> m_gameObjects;

	//gbuffer
	std::unique_ptr<GBuffer> m_gbuffer = nullptr;
	//debug viewer
	std::unique_ptr<DebugViewer> m_debug_viewer = nullptr;

};
