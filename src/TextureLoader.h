#ifndef _TEXTURE_LOADER_H_
#define _TEXTURE_LOADER_H_

#include <string>
#include <memory>
#include <SOIL2.h>
#include <filesystem>
#include "RenderStruct.h"
class TextureLoader
{
public:
	TextureLoader(const std::string& texture_prefix="")
		:m_texture_folder(texture_prefix)
	{
	}

	void setLoadFolder(const std::string& texture_folder)
	{
		m_texture_folder = texture_folder;
	}

	std::unique_ptr<Texture> load(const std::string& filepath)
	{
		std::string fullpath = filepath;
		if(m_texture_folder.size() > 0)
			fullpath = m_texture_folder + "/"+ filepath;

		namespace fs = std::filesystem;
		if(!fs::exists(fullpath))
		{
			fmt::print("texture not found:{}\n",fullpath);
		}

		auto texture_id = SOIL_load_OGL_texture
		(
			filepath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

		std::unique_ptr<Texture> texture(new Texture());
		texture->set(texture_id);
		return texture;
	}

	std::unique_ptr<Texture> genTex(int width,int height,int depth,bool random=true,unsigned char val=255)
	{
		if(!(depth == 1 || depth == 3))
		{
			fmt::print("image format not supported\n");
			throw std::string("image format not supported\n");
		}
		std::unique_ptr<unsigned char[]> img_data(new unsigned char[width * height * depth]);
		if(random)
			for (int i=0;i<width * height * depth;i++)img_data[i] = rand() % 255;
		else
			for (int i=0;i<width * height * depth;i++) img_data[i] = val;

		auto texture_id = SOIL_load_OGL_texture_from_memory(
			img_data.get(),
			width*height*depth,
			depth,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_MIPMAPS);

		std::unique_ptr<Texture> texture(new Texture());
		texture->set(texture_id);
		return texture;
	}
private:
	std::string m_texture_folder = "";
};

#endif
