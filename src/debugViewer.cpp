#include "debugViewer.h"
#include "GameApp.h"

DebugViewer::DebugViewer(GameApp* app)
{
	this->app = app;
}

void DebugViewer::drawTex(unsigned int texid,int x,int y,int width,int height)
{
	if(!shader)
	{
		fmt::print("DebugViewer shader not ready");

		return;
	}
	auto viewsize = app->getCamera()->getScreenSize();
	glm::vec4 rect(x/viewsize.x,y/viewsize.y,2.0f*width/viewsize.x,-2.0f*height/viewsize.y);
	rect.x = 2.0f * rect.x -1.0f;
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
		float stride = ( 2 ) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
	}

	shader->begin();
	shader->setUniformVf("rect",rect);
	shader->setTex2d("diffuse",texid);

	glCullFace(GL_FRONT);
	glBindVertexArray(quad_vao);
	glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
	glCullFace(GL_BACK);

	shader->end();
}