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
#include <pystring.h>



inline GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			//case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			//case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default:
			error = "undefined";
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
};
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

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

	unsigned int setBinary(std::string vs_path,std::string fs_path)
	{
		//https://www.geeks3d.com/20200211/how-to-load-spir-v-shaders-in-opengl/

		std::string vs_spv = vs_path;
		if(!pystring::endswith(vs_path,".spv"))
			vs_spv += ".spv";
		std::string fs_spv = fs_path;
		if(!pystring::endswith(fs_spv,".spv"))
			fs_spv += ".spv";


		//get file content
		auto getFileContent = [](const std::string& filepath,int& byte_length){
			FILE* file = fopen(filepath.c_str(),"rb");
			byte_length = 0;
			if(!file)
				throw std::string(filepath + " not found!");

			fseek(file,0,std::ios::end);
			byte_length = ftell(file);
			fseek(file,0,std::ios::beg);
			std::unique_ptr<unsigned char[]> buf(new unsigned char[byte_length]);
			fread(buf.get(),1,byte_length,file);
			fclose(file);

			return std::move(buf);
		};


		int vs_byte_len = 0;
		auto vs_buf = getFileContent(vs_spv, vs_byte_len);
		int fs_byte_len = 0;
		auto fs_buf = getFileContent(fs_spv,fs_byte_len);

		auto vs_shader = glCreateShader(GL_VERTEX_SHADER);
		auto fs_shader = glCreateShader(GL_FRAGMENT_SHADER);

#pragma region check binary shader support

		// https://docs.microsoft.com/en-us/windows/win32/opengl/using-the-query-functions
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/
		// https://www.jianshu.com/p/e9a9744e318a
		
		//int num = 0;
		//glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &num);
		//if (!num)
		//{
		//	fmt::print("binary shader: not supported");
		//	throw std::string("binary shader: not supported");
		//}

		//std::unique_ptr<int[]> formats(new int[num]);
		//glGetIntegerv(GL_SHADER_BINARY_FORMATS, formats.get());
#pragma endregion



		glShaderBinary(1, &vs_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, vs_buf.get(), vs_byte_len);
		glSpecializeShader(vs_shader, "main", 0, 0, 0);

		

		//get compile status
		int compiled_vs = 0;
		glGetShaderiv(vs_shader, GL_COMPILE_STATUS, &compiled_vs);

		if (compiled_vs != 0)
		{
			std::vector<char> info;
			info.reserve(compiled_vs + 1);
			info.resize(compiled_vs);

			GLsizei len;
			glGetShaderInfoLog(vs_shader, compiled_vs, &len, &info[0]);
			if (info[compiled_vs - 1] != '\0')
			{
				info.push_back('\0');
			}

			std::cout <<vs_spv<< &info[0] << "\n";
		}

		glShaderBinary(1, &fs_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, fs_buf.get(), fs_byte_len);
		glSpecializeShader(fs_shader, "main", 0, 0, 0);
		int compiled_fs = 0;
		glGetShaderiv(fs_shader, GL_COMPILE_STATUS, &compiled_fs);

		if (compiled_fs != 0)
		{
			std::vector<char> info;
			info.reserve(compiled_fs + 1);
			info.resize(compiled_fs);

			GLsizei len;
			glGetShaderInfoLog(fs_shader, compiled_fs, &len, &info[0]);
			if (info[compiled_fs - 1] != '\0')
			{
				info.push_back('\0');
			}

			std::cout <<fs_spv<< &info[0] << "\n";
		}

		//create program
		GLuint prog = glCreateProgram();
		if (prog == 0)
		{
			glDeleteShader(vs_shader);
			glDeleteShader(fs_shader);
			std::cout << "[SHADER]Failed to create program.\n";
			system("pause");
			return 0;
		}
		glAttachShader(prog, vs_shader);
		glAttachShader(prog, fs_shader);
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
			glDeleteShader(vs_shader);
			glDeleteShader(fs_shader);
			std::cout << "Failed to link shader.\n";
			system("pause");
			return 0;
		}

		glDeleteShader(vs_shader);
		glDeleteShader(fs_shader);

		program_id = prog;

		return prog;
	}

	unsigned int setFromFile(std::string vs_path,std::string fs_path)
	{
#if 0
		setBinary(vs_path,fs_path);
#else
		fmt::print("compile shader:\n{} \n{}\n", vs_path, fs_path);
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
#endif
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
		//默认返回false
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

	//!set vector uniforms
	template<size_t N,size_t M>
	bool setUniformVAf(const std::string& paramname, glm::vec<N, float, glm::highp>* val)
	{
#ifndef NDEBUG
		if (!ifUniformExist(paramname))return false;
#endif
		//默认返回false
		switch (N)
		{
		case 2:
			glUniform2fv(glGetUniformLocation(program_id, paramname.c_str()), M, (float*)&val[0]);
			break;
		case 3:
			glUniform3fv(glGetUniformLocation(program_id, paramname.c_str()), M, (float*)&val[0]);
			break;
		case 4:
			glUniform4fv(glGetUniformLocation(program_id, paramname.c_str()), M, (float*)&val[0]);
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


//fbo


//!create fbo
//http://www.mamicode.com/info-detail-1767795.html
class RenderTarget
{
public:
	RenderTarget(int width,int height,const std::vector<unsigned int>& rt_types )
		:m_width(width),m_height(height)
	{
		this->m_tex_types = rt_types;
		createFBO(m_width,m_height);
	};

	void change(int width,int height)
	{
		m_width = (width);
		m_height = (height);
		createFBO(width,height);
		
	}

	~RenderTarget()
	{
		free();
	}

	void bind(bool bindthis=true)
	{
		if(bindthis)
		{
			glBindFramebuffer(GL_FRAMEBUFFER,this->m_fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
			glBindFramebuffer(GL_FRAMEBUFFER,0);
	}

	glm::ivec2 getSize()
	{
		return glm::ivec2(m_width,m_height);
	}
	//get render taget textures;
	std::vector<unsigned int>& getRenderTarget()
	{
		return m_tex_ids;
	}

private:

	void free()
	{
		//free before
		if (m_tex_ids.size() != 0)
		{
			printf("warning colorBuffers'size is not zero,free now\n");
			for (size_t i = 0; i < m_tex_ids.size()-1; i++)
			{
				//cause errors
				glDeleteTextures(1, &m_tex_ids[i]);
			}
			glDeleteRenderbuffers(1,&m_tex_ids[m_tex_ids.size()-1]);

			m_tex_ids.clear();

			//glDeleteFramebuffers(1,&m_fbo);
			//m_fbo = 0;
		}
	}

	inline GLuint createFBO(int width, int height)
	{
		//std::vector<unsigned int> tex_types = {
		//	GL_FLOAT, // position map
		//	GL_FLOAT, // normal map
		//	GL_UNSIGNED_BYTE, // diffuse map
		//};

		//copy

		auto tex_types = m_tex_types;

		free();
		
		GLuint fbo = m_fbo;
		if(m_fbo == 0)
		{
			glGenFramebuffers(1, &fbo);
			m_fbo = fbo;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//color buffer
		for (int i = 0; i < tex_types.size(); i++)
		{
			GLuint colorbuffer;
			glGenTextures(1, &colorbuffer);
			glBindTexture(GL_TEXTURE_2D, colorbuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			if (tex_types[i] == GL_FLOAT)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, tex_types[i], nullptr);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, tex_types[i], nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorbuffer, 0);


			glBindTexture(GL_TEXTURE_2D, 0);
			m_tex_ids.emplace_back(colorbuffer);
		}

		//depth buffer
		GLuint depthrenderbuffer;
		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

		m_tex_ids.emplace_back(depthrenderbuffer);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("FBO Create Fail\n");
		}

		//tell the gpu : what i want to draw
		//GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2 };
		//glDrawBuffers(3, mrt);
		std::vector<GLenum> mrts;
		for (int i=0;i<tex_types.size();i++)
		{
			mrts.emplace_back(GL_COLOR_ATTACHMENT0 + i);
		}

		//mrts.emplace_back(GL_DEPTH_ATTACHMENT);


		glDrawBuffers(tex_types.size(), &mrts[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return fbo;
	};


	//varibles
	
	int m_width,m_height;
	unsigned int m_fbo = 0;
	std::vector<unsigned int> m_tex_types;
	std::vector<unsigned int> m_tex_ids;
};


#endif