#include <fmt/core.h>
#include <RenderItem.h>

Transform::Transform()
{

}
Transform::~Transform()
{
	parent = nullptr;
}

void Transform::setParent(Transform* parent)
{
	this->parent = parent;
}

void Transform::update()
{
	//update world
	auto scale_val = glm::scale(glm::mat4(1.0f),scale);
	auto rotation_val = init_rot * glm::eulerAngleYXZ(rotation.y,rotation.x,rotation.z);
	auto translate_val = glm::translate(glm::mat4(1.0f),position);

	//opengl 从右往左
	world = translate_val * rotation_val  * scale_val;

	//update view
	forward = glm::vec3(0,0,1);
	forward = glm::mat3(world) * forward;

	right = glm::normalize(glm::cross(forward,up));

	if(parent)
	{
		world = parent->world * world;
	}
}


RenderItem::RenderItem()
{
	mat = new Material();
	mat->diffuse = glm::vec3(1,1,1);
	mat->specular = glm::vec3(1,1,1);
}

RenderItem::~RenderItem()
{
	if(mat)
		delete mat;
}


void MeshGeometry::setBuffer(const std::vector<GeometryGenerator::MeshData>& mesh_datas)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (auto mesh_data : mesh_datas)
	{
		//新的起始index
		unsigned int base_start_idx = vertices.size();

		for(auto vertex : mesh_data.vertices)
		{
			Vertex v;
			v.pos = vertex.position;
			v.normal = vertex.normal;
			v.uv = vertex.uv;

			vertices.emplace_back(v);
		}

		for (auto idx:mesh_data.indices)
		{
			auto real_idx = idx + base_start_idx;

			indices.emplace_back(real_idx);
		}

		SubmeshGeometry sg;
		sg.index_count = mesh_data.indices.size();
		sg.index_start = indices.size() - mesh_data.indices.size();
		sg.vertex_start = base_start_idx;


		this->drawing_args[mesh_data.name]  = sg;
	}

	//update VB and IB
	if(!vertex_array)
	{
		glGenVertexArrays(1,&vertex_array);
	}

	glBindVertexArray(vertex_array);

	if(vertex_buffer) glDeleteBuffers(1,&vertex_buffer);
	if(index_buffer) glDeleteBuffers(1,&index_buffer);
	glGenBuffers(1,&vertex_buffer);
	glGenBuffers(1,&index_buffer);
	

	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex),&vertices[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),&indices[0],GL_STATIC_DRAW);
}


void RenderItem::render(FPSCamera* camera)
{
	if(shader && mesh)
	{
		shader->begin();

		//only for desktop
		glBindVertexArray(mesh->vertex_array);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);

		//attributes
		auto att_pos = glGetAttribLocation(shader->program_id, "position");
		auto att_normal = glGetAttribLocation(shader->program_id, "normal");
		auto att_uv = glGetAttribLocation(shader->program_id, "uv");
		glEnableVertexAttribArray(att_pos);
		glVertexAttribPointer(att_pos,3,GL_FLOAT,GL_FALSE,12+12+8,(void*)0);
		glEnableVertexAttribArray(att_normal);
		glVertexAttribPointer(att_normal, 3, GL_FLOAT, GL_FALSE, 12 + 12 + 8, (void*)12);
		glEnableVertexAttribArray(att_uv);
		glVertexAttribPointer(att_uv, 3, GL_FLOAT, GL_FALSE, 12 + 12 + 8, (void*)24);

		//uniforms
		shader->setUniformMf("model",transform.world);
		if(camera)
		{
			shader->setUniformMf("view", camera->getViewMatrix());
			shader->setUniformMf("proj", camera->getProjectionMatrix());
			shader->setUniformVf("cam_pos", camera->getPosition());
			shader->setUniformVf("viewport_size", camera->getScreenSize());
		}
		//mat
		shader->setUniformInt("use_tex",tex_diffuse?1:0);
		shader->setUniformVf("diffuse_color",this->mat->diffuse);

		//pbr texture setting
		if (tex_diffuse) shader->setTex2d("tex_diffuse",tex_diffuse->tex_id); else shader->setTex2d("tex_diffuse",0);
		if (tex_normal) shader->setTex2d("tex_normal", tex_normal->tex_id);else shader->setTex2d("tex_normal",0);
		if (tex_ao) shader->setTex2d("tex_ao", tex_ao->tex_id);else shader->setTex2d("tex_ao",0);
		if (tex_matallic) shader->setTex2d("tex_matallic", tex_matallic->tex_id);else shader->setTex2d("tex_matallic",0);
		if (tex_roughness) shader->setTex2d("tex_roughness", tex_roughness->tex_id);else shader->setTex2d("tex_roughness",0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh->index_buffer);
		for (auto& [name,geo] : mesh->drawing_args)
		{
			//fmt::print("draw {}\n",name);
			glDrawElements(GL_TRIANGLES,
				geo.index_count,
				GL_UNSIGNED_INT,
				(void*)geo.index_start);
		}

		shader->end();
	}
}

