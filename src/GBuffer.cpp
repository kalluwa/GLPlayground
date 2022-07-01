#include "GBuffer.h"
#include "fmt/core.h"

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
}

void GBuffer::build(int width,int height)
{
	render_target = new RenderTarget(width,height);

}

void GBuffer::begin()
{
	if(!render_target)
	{
		fmt::print("error:render target not created!");
		return;
	}

	render_target->bind(true);


}

void GBuffer::end()
{
	render_target->bind(false);
}

void GBuffer::draw(std::function<void(Shader* shader)> callback)
{
	if(!shader)
	{
		fmt::print("gbuffer shader not set");
		return;
	}

	begin();

	//set shader first
	callback(shader);

	end();
}