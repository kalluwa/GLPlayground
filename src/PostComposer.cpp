#include "PostComposer.h"
#include "GameApp.h"

void PostPass::draw(FPSCamera* cam)
{
	if (!shader)
	{
		fmt::print("DebugViewer shader not ready");

		return;
	}
	auto viewsize = cam->getScreenSize();
	auto cam_pos = cam->getPosition();

	glm::vec4 rect(0, 0, 2, -2);
	rect.x = 2.0f * rect.x - 1.0f;
	rect.y = -(2.0f * rect.y - 1.0f);

	//fmt::print("rect {} {} {} {}\n",rect[0],rect[1],rect[2],rect[3]);
	static unsigned int quad_vao = 0;
	static unsigned int index_count;
	if (quad_vao == 0)
	{
		glGenVertexArrays(1, &quad_vao);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec2> positions;
		std::vector<unsigned int> indices;

		positions.emplace_back(glm::vec2(0, 0));
		positions.emplace_back(glm::vec2(0, 1));
		positions.emplace_back(glm::vec2(1, 1));
		positions.emplace_back(glm::vec2(1, 0));

		indices.emplace_back(0);
		indices.emplace_back(1);
		indices.emplace_back(2);


		indices.emplace_back(0);
		indices.emplace_back(2);
		indices.emplace_back(3);
		index_count = indices.size();


		glBindVertexArray(quad_vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), &positions[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (2) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
	}

	shader->begin();
	shader->setUniformVf("rect", rect);
	shader->setUniformVf("viewport_size", viewsize);
	shader->setUniformVf("cam_pos", cam_pos);

	shader->setUniformVAf<3,64>("offset_positions",this->offset_positions);
	auto view = cam->getViewMatrix();
	shader->setUniformMf("view", view);
	auto proj = cam->getProjectionMatrix();
	shader->setUniformMf("proj", proj);

	auto inv_view = glm::inverse(view);
	shader->setUniformMf("inv_view",inv_view);

	if (textures.size())
	{
		for (auto& [key, val] : textures)
			shader->setTex2d(key, val);
	}
	else
		fmt::print("[postpass] screen texture isn't set before");

	glCullFace(GL_FRONT);
	glBindVertexArray(quad_vao);
	glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
	glCullFace(GL_BACK);

	shader->end();
};


PostComposer::PostComposer(GameApp* app)
{
	this->app = app;

	auto viewsize = app->getViewport();
	rt = new RenderTarget(viewsize[0], viewsize[1],{GL_UNSIGNED_BYTE});
}

PostComposer::~PostComposer()
{
	if (rt)
		delete rt;
}

void PostComposer::draw()
{
	if(!app || m_passes.size()==0)
		return;

	//是否要重新resize
	auto viewport = app->getViewport();
	if (rt->getSize() != viewport)
		rt->change(viewport.x, viewport.y);

	auto cam = app->getCamera();
	//处理一个pass的情况：不用rendertaget
	if(m_passes.size() == 1)
		m_passes[0]->draw(cam);
	//处理多个pass:前面n-1个用rendertarget
	else
	{
		//记录前面n个rendertaget
		for(int i=0;i<m_passes.size()-1;i++)
		{
			rt->bind();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_passes[i]->draw(cam);
			rt->bind(false);

			//set rendertaget to next pass
			auto texes = rt->getRenderTarget();
			if(texes.size()>0)
				//m_passes[i+1]->shader->setTex2d("rt_map",texes[0]);
				m_passes[i+1]->textures["rt_map"] = texes[0];
			else
			{
				fmt::print("Error:rendertaget is empty");
				exit(0);
			}
		}

		//最后一个pass:绘制到屏幕上
		auto& lastpass = m_passes[m_passes.size()-1];
		//auto texes = rt->getRenderTarget();
		//lastpass->shader->setTex2d("rt_map",texes[0]);
		//lastpass->textures["rt_map"] = texes[0];
		lastpass->draw(cam);

	}
}