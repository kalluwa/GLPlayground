#include "PostComposer.h"
#include "GameApp.h"

void PostPass::draw(FPSCamera* cam)
{
	if (!shader)
	{
		fmt::print("POSTPass shader not ready");

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

	//shader->setUniformVAf<3,64>("offset_positions",this->offset_positions);
	auto view = cam->getViewMatrix();
	shader->setUniformMf("view", view);
	auto proj = cam->getProjectionMatrix();
	shader->setUniformMf("proj", proj);

	auto inv_view = glm::inverse(view);
	shader->setUniformMf("inv_view",inv_view);

	if (textures.size())
	{
		for (auto& [key,tex_uint] : textures)
			if(tex_uint.tex_type == GL_TEXTURE_2D_ARRAY)
				shader->setTex2dArray(key, tex_uint.tex_id);
			else
				shader->setTex2d(key, tex_uint.tex_id);
	}
	//else
	//	fmt::print("[postpass] screen texture isn't set before");

	glCullFace(GL_FRONT);
	glBindVertexArray(quad_vao);
	glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
	glCullFace(GL_BACK);

	shader->end();
};


PostComposer::PostComposer(GameApp* app)
{
	this->app = app;

	
}

PostComposer::~PostComposer()
{
	for(auto& rt : rts)
		delete rt;
	rts.clear();
}

void PostComposer :: add(std::unique_ptr<PostPass>&& pass)
{
	m_passes.emplace_back(std::move(pass));

	//how many rts can exist
	int continouscount = 0;
	int max_continouscount = 0;
	for (auto& pass : m_passes)
	{
		if (pass->passthrough)
			continouscount = 1;
		else
			continouscount++;

		if (max_continouscount < continouscount)
			max_continouscount = continouscount;
	}
	if (max_continouscount != rts.size())
	{
		auto viewsize = app->getViewport();
		//添加补充的rt
		for (int i = rts.size(); i < max_continouscount; i++)
		{
			RenderTarget* rt = new RenderTarget(viewsize[0], viewsize[1], { GL_UNSIGNED_BYTE },true);
			rts.emplace_back(rt);
		}
	}
}
void PostComposer::draw()
{
	if(!app || m_passes.size()==0)
		return;

	//是否要重新resize
	auto viewport = app->getViewport();
	if (rts[0]->getSize() != viewport)
		for(auto& rt : rts)
			rt->change(viewport.x, viewport.y);

	auto cam = app->getCamera();
	//处理一个pass的情况：不用rendertaget
	if(m_passes.size() == 1)
		m_passes[0]->draw(cam);
	//处理多个pass:前面n-1个用rendertarget
	else
	{
		std::vector<unsigned int> texes;
		//记录前面n个rendertaget
		for(int i=0;i<m_passes.size()-1;i++)
		{
			if(m_passes[i]->passthrough)
				rts[0]->bind();
			else rts[1]->bind();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_passes[i]->draw(cam);
			if(m_passes[i]->passthrough)
				rts[0]->bind(false);
			else rts[1]->bind(false);

			//set rendertaget to next pass

			std::vector<unsigned int> tmp_texes;
			if(m_passes[i]->passthrough)
				texes = rts[0]->getRenderTarget();
			else
				tmp_texes = rts[1]->getRenderTarget();

			if(texes.size()>0)
				//m_passes[i+1]->shader->setTex2d("rt_map",texes[0]);
				m_passes[i+1]->textures["rt_map"] = texes[0];
			else
			{
				fmt::print("Error:rendertaget is empty");
				exit(0);
			}

			if(tmp_texes.size())
			{
				m_passes[i+1]->textures[m_passes[i]->tagname] = tmp_texes[0];
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

AOPass::AOPass(GameApp* app)
{
	passthrough = false;
	tagname = "aomap";

	//ssao shader & ssao blur shader
	std::unique_ptr<Shader> ssao_shader(new Shader);
	ssao_shader->setFromFile(R"(resources\shaders\ssao\9.ssao.vs)",
		R"(resources\shaders\ssao\9.ssao.fs)");


	std::unique_ptr<Shader> ssaoblur_shader(new Shader);
	ssaoblur_shader->setFromFile(R"(resources\shaders\ssao\9.ssao.vs)",
		R"(resources\shaders\ssao\9.ssao_blur.fs)");

	//assign to shader repos
	app->getShaders()["ssao"] = std::move(ssao_shader);
	app->getShaders()["ssaoblur"] = std::move(ssaoblur_shader);

	this->shader = app->getShaders()["ssao"].get();
	this->blur_shader = app->getShaders()["ssaoblur"].get();
	this->textures.clear();

	//create ssao kernel
	// ----------------------
	auto lerp = [](float a, float b, float f){
		return a + f * (b - a);
	};
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator) * 2.0 - 1.0, 
			randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	//create noise texture
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D,0);
	
	this->shader->begin();
	//this->shader->setUniformVAf<3,64>("samples",&ssaoKernel[0]);
	for (unsigned int i = 0; i < 64; ++i)
		this->shader->setUniformVf("samples[" + std::to_string(i) + "]", ssaoKernel[i]);

	this->shader->end();
}
void AOPass::draw(FPSCamera* cam)
{
	//配置shader
	//shader->begin();
	//shader->setUniformInt("gPosition", pos_map);//贴图
	//shader->setUniformInt("gNormal", normal_map);//贴图
	//shader->setUniformInt("texNoise", noiseTexture);//贴图
	//shader->end();
	textures["gPosition"] = TextureUint(pos_map);
	textures["gNormal"] = TextureUint(normal_map);
	textures["texNoise"] = TextureUint(noiseTexture);

	this->shader->begin();
	this->shader->setUniformMf("projection",cam->getProjectionMatrix());
	this->shader->end();

	PostPass::draw(cam);

	//shader->end();
	//blur_shader->begin();
	//blur_shader->setUniformInt("ssaoInput", 0);//贴图

}