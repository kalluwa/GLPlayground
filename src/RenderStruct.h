#ifndef _RENDER_STRUCT_H_
#define _RENDER_STRUCT_H_

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct Material
{
	Material()
	{
		specular = diffuse = glm::vec3(0,0,0);
	}
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct DirectLight
{
	DirectLight()
	{
		light_color = glm::vec3(1,1,1);
		light_dir = glm::vec3(-1,-1,-1);
	}
	glm::vec3 light_color;
	glm::vec3 light_dir;
};

struct Shader
{
	Shader()
	{
		program_id = 0;
	}

	unsigned int set(std::string vs_str,std::string fs_str)
	{
#pragma region createShader
		auto createShader = [](GLenum shaderType, const std::string code)->unsigned int
		{
			GLuint shader = glCreateShader(shaderType);
			if (shader == 0)
			{
				std::cout << "Failed to create shader.\n";
				return 0;
			}
			const char* codes[] = { code.c_str() };
			GLint codesLen[] = { (GLint)code.size() };
			glShaderSource(shader, 1, codes, codesLen);
			glCompileShader(shader);

			GLint infoLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
			if (infoLength != 0)
			{
				std::vector<char> info;
				info.reserve(infoLength + 1);
				info.resize(infoLength);

				GLsizei len;
				glGetShaderInfoLog(shader, infoLength, &len, &info[0]);
				if (info[infoLength - 1] != '\0')
				{
					info.push_back('\0');
				}

				std::cout << &info[0] << "\n";
			}

			GLint compileStatus;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
			if (compileStatus != GL_TRUE)
			{
				glDeleteShader(shader);
				std::cout << "Failed to compile shader.\n";
				return 0;
			}

			return shader;
		};
#pragma endregion

		auto vs = createShader(GL_VERTEX_SHADER, vs_str);
		auto fs = createShader(GL_FRAGMENT_SHADER, fs_str);
		if (vs == 0 || fs == 0)
		{
			if (vs != 0) { glDeleteShader(vs); }
			if (fs != 0) { glDeleteShader(fs); }
			return 0;
		}

		GLuint prog = glCreateProgram();
		if (prog == 0)
		{
			glDeleteShader(vs);
			glDeleteShader(fs);
			std::cout << "[SHADER]Failed to create program.\n";
			system("pause");
			return 0;
		}
		glAttachShader(prog, vs);
		glAttachShader(prog, fs);
		glLinkProgram(prog);

		GLint infoLength;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength != 0)
		{
			std::vector<char> info;
			info.reserve(infoLength + 1);
			info.resize(infoLength);

			GLsizei len;
			glGetProgramInfoLog(prog, infoLength, &len, &info[0]);
			if (info[infoLength - 1] != '\0')
			{
				info.push_back('\0');
			}

			std::cout << &info[0] << "\n";
		}

		GLint linkStatus;
		glGetProgramiv(prog, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			glDeleteShader(vs);
			glDeleteShader(fs);
			std::cout << "Failed to link shader.\n";
			system("pause");
			return 0;
		}

		glDeleteShader(vs);
		glDeleteShader(fs);

		this->vs_str = vs_str;
		this->fs_str = fs_str;

		program_id = prog;
		return prog;

	}

	unsigned int setFromFile(std::string vs_path,std::string fs_path)
	{
		auto getStr = [](std::string filepath)->std::string
		{
			std::ifstream ifs;
			ifs.open(filepath);
			if(!ifs.is_open())
			{
				std::cout<<filepath<<" not exist"<<std::endl;
				return 0;
			}
			std::stringstream ss;
			std::string line;
			while(std::getline(ifs,line))
			{
				ss << line;
			}

			std::string filestr;
			ss >> filestr;
			//clear
			ss.str("");
			return filestr;
		};

		set(getStr(vs_path),getStr(fs_path));
	};
	std::string vs_str;
	std::string fs_str;

	unsigned int program_id;
};


struct Texture
{
	Texture()
	{
		tex_id = 0;
	}

	void set(unsigned int id){ tex_id = id;}

	unsigned int tex_id;
};

struct ObjectUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model_inv;

	glm::mat4 shadow_transform;
	glm::mat4 uv_st;

};

struct SkyboxUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

struct SSAOUniform
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 view_inv;
	glm::vec4 offset[64];
};

struct ObjectLight
{
	DirectLight dir_light;
	Material mat;
	glm::vec3 eye_pos;
};
#endif