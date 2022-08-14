#include "test_unit.h"

#include "TextureLoader.h"
#include "GeometryGenerator.h"

#include <fmt/core.h>

void TestUnit::TestLoadTexture()
{
	auto loader = new TextureLoader();
	auto tex =loader ->load(R"(D:\projects\DX11-Car-Demo-master\Project1\Tex\floor.dds)");
	printf("texture load result:%d\n",tex->tex_id);
}

void TestUnit::TestLoadObj()
{
	auto gg = new GeometryGenerator();
	GeometryGenerator::MeshData md;
	std::string filename = R"(D:\games\organs_models\beihang\liver.obj)";
	if(!gg->loadFromFile(filename,md))
	{
		fmt::print("loaded failed\n");
	}
	else
	{
		fmt::print("loaded passed\n");
	}
}

void TestUnit::runAll()
{
	//TestLoadTexture();
	//TestLoadObj();

	//int buf[256] = {0};
	//
	//int num = 0;
	//glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,&num);
	//if(num)
	//{
	//	std::unique_ptr<int[]> formats (new int[num]);
	//	glGetIntegerv(GL_SHADER_BINARY_FORMATS,formats.get());

	//	int a = 0;
	//}
}
