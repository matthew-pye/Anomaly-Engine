#include "aepch.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <Anomaly/Rendering/stb_image.h>

#include <glad/glad.h>

namespace Anomaly
{
	void Shader::ReadInShaders(const char* VertexSrcFileName, const char* FragmentSrcFileName)
	{
		std::string AppPathVert = __argv[0];	
		AppPathVert.replace(AppPathVert.end() - 11, AppPathVert.end(), "Shaders\\" + std::string(VertexSrcFileName));
		const char* vPath = AppPathVert.c_str();
		
		std::string AppPathFrag = __argv[0];
		AppPathFrag.replace(AppPathFrag.end() - 11, AppPathFrag.end(), "Shaders\\" + std::string(FragmentSrcFileName));
		const char* fPath = AppPathFrag.c_str();
		

		std::ifstream VertexShaderFile;
		std::ifstream FragmentShaderFile;

		VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			std::stringstream ShaderStreamVert, ShaderStreamFrag;
			
			VertexShaderFile.open(vPath);
			ShaderStreamVert << VertexShaderFile.rdbuf();		
			VertexSrc = ShaderStreamVert.str();
			VertexShaderFile.close();

			FragmentShaderFile.open(fPath);
			ShaderStreamFrag << FragmentShaderFile.rdbuf();
			FragmentSrc = ShaderStreamFrag.str();
			FragmentShaderFile.close();
			
		}
		catch (std::ifstream::failure e)
		{
			AE_CORE_ERROR("Shader File failed to read!: {0}", e.what());
		}
	}
	
	Shader::Shader(const char* VertexSrcFileName,const char* FragmentSrcFileName)
	{
		ReadInShaders(VertexSrcFileName, FragmentSrcFileName);
		
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		
		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = static_cast<const GLchar *>(VertexSrc.c_str());
		glShaderSource(vertexShader, 1, &source, nullptr);
		
		// Compile the vertex shader
		glCompileShader(vertexShader);
		
		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
		
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			AE_CORE_ERROR("{0}", infoLog.data());
			AE_CORE_ASSERT(false, "Vertex Shader compilation failed!")
			
			return;
		}
		
		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = static_cast<const GLchar *>(FragmentSrc.c_str());
		glShaderSource(fragmentShader, 1, &source, 0);
		
		// Compile the fragment shader
		glCompileShader(fragmentShader);
		
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
		
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);
		
			AE_CORE_ERROR("{0}", infoLog.data());
			AE_CORE_ASSERT(false, "Fragment Shader compilation failed!")
			
			// In this simple program, we'll just leave
			return;
		}
		
		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererID = glCreateProgram();
		
		// Attach our shaders to our program
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		
		// Link our program
		glLinkProgram(m_RendererID);
		
		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);
		
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		
			// Use the infoLog as you see fit.
			
			AE_CORE_ERROR("{0}", infoLog.data());
			AE_CORE_ASSERT(false, "Shaders Linking failed!")
			// In this simple program, we'll just leave
			return;
		}
		
		// Always detach shaders after a successful link.
		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);

	}
	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	void Shader::UnBind() const
	{
		glUseProgram(0);
	}

	void Shader::GenerateTextures()
	{
		//Generates and binds 1st texture
		std::string AppPath = __argv[0];	
		AppPath.replace(AppPath.end() - 11, AppPath.end(), "Textures\\container.jpg");
		const char* tPath = AppPath.c_str();

		glGenTextures(1, &texture0);
		glBindTexture(GL_TEXTURE_2D, texture0);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrchannels;
		stbi_set_flip_vertically_on_load(true); 
		unsigned char *data = stbi_load(tPath, &width, &height, &nrchannels, 0);

		if(data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			AE_CORE_ERROR("Texture failed to load!");
		}	
		stbi_image_free(data);

		//TODO: Setup the ability for multiple textures to be used in one shader
	}
	void Shader::BindTextures()
	{		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		SetUniformInt("u_Texture0", 0);
	}

	void Shader::SetUniformMatrix4(const std::string& name, const glm::mat4& matrix)
	{	
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1,	GL_FALSE ,glm::value_ptr(matrix));
	}
	void Shader::SetUniformBool(const std::string& name, bool value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}
	void Shader::SetUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}
	void Shader::SetUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}
}
