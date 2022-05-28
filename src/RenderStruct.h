#ifndef _RENDER_STRUCT_H_
#define _RENDER_STRUCT_H_

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <fmt/core.h>

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
		//fmt::print("--vs--\n{}\n----\n",vs_str);

		//fmt::print("--fs--\n{}\n----\n",vs_str);
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
			std::string ss;
			std::string line;
			while(std::getline(ifs,line))
			{
				ss += line + "\n";
			}

			//std::string filestr;
			//clear
			return ss;
		};

		set(getStr(vs_path),getStr(fs_path));

		return program_id;
	};
	std::string vs_str;
	std::string fs_str;

	//!set float uniform
	bool setUniformF(const std::string& paramname, float val)
	{
		glUniform1f(glGetUniformLocation(program_id, paramname.c_str()), val);

		return true;
	};


	bool setUniformInt(const std::string& paramname, int val) 
	{
		glUniform1i(glGetUniformLocation(program_id, paramname.c_str()), val);

		return true;
	};


	//! debug:check uniform
	bool ifUniformExist(const std::string& paramname)
	{
		if (-1 == glGetUniformLocation(program_id, paramname.c_str()))
		{
			//printf("warning: uniform [%s] doesn't exist!\n", paramname.c_str());
			return false;
		};
		return true;
	};

	//{Template member functions need to be in header files}

	//!set vector uniforms
	template<size_t N>
	bool setUniformVf(const std::string& paramname, glm::vec<N, float, glm::highp> val)
	{
#ifndef NDEBUG
		if (!ifUniformExist(paramname))return false;
#endif
		//Ĭ�Ϸ���false
		switch (N)
		{
		case 2:
			glUniform2fv(glGetUniformLocation(program_id, paramname.c_str()), 1, &val[0]);
			break;
		case 3:
			glUniform3fv(glGetUniformLocation(program_id, paramname.c_str()), 1, &val[0]);
			break;
		case 4:
			glUniform4fv(glGetUniformLocation(program_id, paramname.c_str()), 1, &val[0]);
			break;
		default:
			return false;
		}

		return true;
	};

	template<size_t N>
	bool setUniformi(const std::string& paramname, glm::vec<N, int, glm::highp> val)
	{
#ifndef NDEBUG
		if (!ifUniformExist(paramname))return false;
#endif
		switch (N)
		{
		case 2:
			glUniform2iv(glGetUniformLocation(program_id, paramname.c_str()), &val[0]);
			break;
		case 3:
			glUniform3iv(glGetUniformLocation(program_id, paramname.c_str()), &val[0]);
			break;
		case 4:
			glUniform4iv(glGetUniformLocation(program_id, paramname.c_str()), &val[0]);
			break;
		default:
			return false;
		}
		return true;
	};
	//!set matrix uniforms
	template<size_t N>
	bool setUniformMf(const std::string& paramname, glm::mat<N, N, float, glm::highp> val)
	{
#ifndef NDEBUG
		if (!ifUniformExist(paramname))return false;
#endif
		switch (N)
		{
		case 3:
			glUniformMatrix3fv(glGetUniformLocation(program_id, paramname.c_str()), 1, false, &val[0][0]);
			break;
		case 4:
			glUniformMatrix4fv(glGetUniformLocation(program_id, paramname.c_str()), 1, false, &val[0][0]);
			break;
		}

		return true;
	};

	//!return opengl program id
	GLuint getId() const { return program_id; };
	void begin() {
		glUseProgram(program_id);
		cur_tex_loc = 0;
	};

	void end() {
		glUseProgram(0);
	};

	//!set attrib
	//@paramname : attrib name
	//@array_buf_id: array buffer id ( vbo )
	//@size_element : how many element in attrib [eg:normal->3 uv->2]
	//@size_type : GL_FLOAT or GL_UNSIGNED_INT or GL_UNSIGNED_BYTE etc
	//@b_neednormalized: GL_FALSE or GL_TRUE
	//@size_of_vertex : size of vertex [0 means auto detect]
	//@param_offset : offset to vertex start 
	void bindAttrib(const std::string& paramname, unsigned int array_buf_id, int size_element, int size_type = GL_FLOAT, int b_neednormalized = GL_FALSE,
		int size_of_vertex = 0, int param_offset = 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, array_buf_id);
		auto pos = glGetAttribLocation(program_id, paramname.c_str());
		if (pos == -1)
		{
			//printf("set attrib [%s] failed\n",paramname.c_str());
			return;
		}
		glEnableVertexAttribArray(pos);
		glVertexAttribPointer(
			pos,
			size_element,
			size_type,
			b_neednormalized,
			size_of_vertex,
			(void*)param_offset);
	};

	void bindIndices(unsigned int indices_id)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	}

	//!set universal texture2d [ make sure enable texture2d called before]
	//@paramname : texture param name
	//@tex_id : texture address id
	void setTex2d(const std::string& paramname, unsigned int tex_id)
	{
#ifndef NDEBUG
		if (!ifUniformExist(paramname))return;
#endif
		glActiveTexture(GL_TEXTURE0 + cur_tex_loc);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glUniform1i(glGetUniformLocation(program_id, paramname.c_str()), cur_tex_loc);

		cur_tex_loc++;
	};

	void setTexCube(const std::string& paramname, unsigned int tex_id)
	{
#ifndef NDEBUG
		ifUniformExist(paramname);
#endif
		glActiveTexture(GL_TEXTURE0 + cur_tex_loc);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
		glUniform1i(glGetUniformLocation(program_id, paramname.c_str()), cur_tex_loc);

		cur_tex_loc++;
	};

	unsigned int program_id = 0;
	unsigned int cur_tex_loc = 0;
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