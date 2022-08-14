#ifndef _RENDER_ITEM_H_
#define _RENDER_ITEM_H_

#include <iostream>
#include <vector>
#include "RenderStruct.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <GeometryGenerator.h>
#include <camera.h>

//#pragma pack(push,1)
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};
//#pragma pack(pop)

struct SubmeshGeometry
{
	unsigned int index_count = 0;
	unsigned int index_start = 0;
	int vertex_start = 0;
};

struct MeshGeometry
{
	std::string name;
	unsigned int vertex_buffer = 0;
	unsigned int index_buffer = 0;
	unsigned int vertex_array = 0;

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	std::unordered_map<std::string,SubmeshGeometry> drawing_args;

	//将多个mesh数据拼接为一个
	void setBuffer(const std::vector<GeometryGenerator::MeshData>& mesh_datas);
};

class Transform
{
public:
	Transform();
	~Transform();

	void setParent(Transform* parent);
	void update();

	glm::vec3 position=glm::vec3(0,0,0);
	glm::vec3 rotation=glm::vec3(0,0,0);
	glm::vec3 scale = glm::vec3(1,1,1);

	glm::vec3 forward = glm::vec3(0, 0, 1);
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = glm::vec3(1,0,0);

	glm::mat4 init_rot = glm::mat4(1.0f);
	glm::mat4 world = glm::mat4(1.0f);
	
	Transform* parent = nullptr;
};


class RenderItem
{
public:
	RenderItem();
	~RenderItem();

	unsigned int primitive_type = GL_TRIANGLES;
	Material* mat = nullptr;
	MeshGeometry* mesh = nullptr;
	
	Texture* tex_diffuse = nullptr;
	Texture* tex_normal = nullptr;
	Texture* tex_ao = nullptr;
	Texture* tex_matallic = nullptr;
	Texture* tex_roughness = nullptr;

	Shader* shader = nullptr;

	unsigned int index_count = 0;
	unsigned int index_start = 0;
	int vertex_start = 0;


	//void setMeshArgs(std::string submeshname);
	
	Transform transform;

	void render(Camera* camera);
};
#endif // _RENDER_ITEM_H_
