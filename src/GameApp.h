#include <unordered_map>
#include <string>
#include <iostream>

#include <RenderItem.h>
#include <camera.h>


class GameApp
{
public:
	GameApp(int w=800,int h=600);
	~GameApp();

	//��Ҫ����
	//��ʼ��
	virtual bool init();
	//����
	virtual void update(float delta_time);
	//����
	virtual void draw();

	virtual void drawGeometry();

	//!input mouse
	void inputMouse(int x, int y, int mouse_button, int mouse_press_or_release);

	void inputMouseScroll(int x);

	//!input keyboard
	void inputKeyboard(int keycode, int action);

	Camera* getCamera(){ return m_camera.get();}
protected:
	//��ȡ��ʼ��ģ������ ��������
	virtual void loadContent();

protected:
	//�洢��������

	//����
	std::unordered_map<std::string,std::unique_ptr<MeshGeometry>> m_geometries;
	//����
	std::unordered_map<std::string,std::unique_ptr<Material>> m_materials;
	//����
	std::unordered_map<std::string,std::unique_ptr<Texture>> m_textures;
	//shader
	std::unordered_map<std::string,std::unique_ptr<Shader>> m_shaders;

	glm::vec2 m_viewport;
	std::unique_ptr<Camera> m_camera = nullptr;

	//��ά����
	std::vector<std::shared_ptr<RenderItem>> m_gameObjects;
};
