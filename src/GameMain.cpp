#include <unordered_map>
#include <string>
#include <iostream>

#include "renderitem.h"


enum CameraMode
{
	First,
	Third
};


class GameMain
{
public:
	GameMain();
	~GameMain();

	//主要方法
	bool init();
	void update(float delta_time);
	void draw();

	void onMouseMove(int btn_state,int x,int y);
	void onMouseWheel(int btn_state);

	//主角，相机，灯光控制
	void playerControl(float delta_time);
	void cameraControl(float delta_time);
	void lightControl(float delta_time);

private:
	//读取初始的模型数据
	void loadGeometry();
	//读取纹理数据
	void loadTexture();

private:
	//存储所有数据

	//几何
	std::unordered_map<std::string,std::unique_ptr<MeshGeometry>> m_geometries;
	//材质
	std::unordered_map<std::string,std::unique_ptr<Material>> m_materials;
	//纹理
	std::unordered_map<std::string,std::unique_ptr<Texture>> m_textures;
	//shader
	std::unordered_map<std::string,std::unique_ptr<Shader>> m_shaders;

	std::unordered_map<std::string,RenderItem> m_renderitems;
};
