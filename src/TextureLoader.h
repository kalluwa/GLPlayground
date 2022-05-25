#ifndef _TEXTURE_LOADER_H_
#define _TEXTURE_LOADER_H_

#include <string>
#include <memory>
#include <SOIL2.h>
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

private:
	std::string m_texture_folder = "";
};

#endif
