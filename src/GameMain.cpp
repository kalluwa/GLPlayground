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

	//��Ҫ����
	bool init();
	void update(float delta_time);
	void draw();

	void onMouseMove(int btn_state,int x,int y);
	void onMouseWheel(int btn_state);

	//���ǣ�������ƹ����
	void playerControl(float delta_time);
	void cameraControl(float delta_time);
	void lightControl(float delta_time);

private:
	//��ȡ��ʼ��ģ������
	void loadGeometry();
	//��ȡ��������
	void loadTexture();

private:
	//�洢��������

	//����
	std::unordered_map<std::string,std::unique_ptr<MeshGeometry>> m_geometries;
	//����
	std::unordered_map<std::string,std::unique_ptr<Material>> m_materials;
	//����
	std::unordered_map<std::string,std::unique_ptr<Texture>> m_textures;
	//shader
	std::unordered_map<std::string,std::unique_ptr<Shader>> m_shaders;

	std::unordered_map<std::string,RenderItem> m_renderitems;
};
