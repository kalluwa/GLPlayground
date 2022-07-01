#ifndef _G_BUFFER_H_
#define _G_BUFFER_H_

//法线world_normal、空间位置world_pos、漫反射diffuse
#include "RenderStruct.h"
#include <functional>

//const int BUFFER_COUNT = 3;
class GBuffer
{
public:
	~GBuffer();

	void build(int width,int height);

	//world_pos,world_normal,diffuse
	//unsigned int textures[BUFFER_COUNT] = {0};

	void begin();
	void draw(std::function<void(Shader*)> callback);
	void end();

	RenderTarget* render_target = nullptr;
	Shader* shader = nullptr;
};
#endif
