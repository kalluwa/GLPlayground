#ifndef _RENDER_ITEM_H_
#define _RENDER_ITEM_H_

#include <iostream>
#include <vector>
#include "RenderStruct.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 uv;
};

struct SubmeshGeometry
{
	unsigned int index_count = 0;
	unsigned int index_start = 0;
	int vertex_start = 0;
};

struct MeshGeometry
{
	std::string name;
	unsigned int vertex_buffer;
	unsigned int index_buffer;

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	std::unordered_map<std::string,SubmeshGeometry> drawing_args;
	//void setBuffer();
};

class RenderItem
{
public:
	RenderItem();
	~RenderItem();

	unsigned int primitive_type = GL_TRIANGLES;
	Material* mat = nullptr;
	MeshGeometry* mesh = nullptr;
	Texture* tex = nullptr;
	Shader* shader = nullptr;

	unsigned int index_count = 0;
	unsigned int index_start = 0;
	int vertex_start = 0;


	void setMeshArgs(std::string submeshname);
	
};
#endif // _RENDER_ITEM_H_
