
#include "REWGE.h"

//#include <ctime>
//#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <stack>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <functional>

#include "REWCPPL.h"
#include "Utils.h"

#include "Memory/Allocator.h"
//#include "Memory//StackAllocator.hpp"


#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Texture.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"


#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"


#include "Shader.h"


// --------------------------------- UTILS ---------------------------------------


//检查Opengl错误
bool checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		cout << "glError:" << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

//打印程序LOG
void printProgramLog(int program)
{
	int len = 0;
	int chWritten = 0;
	char* log;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)Allocator::GetAllocator()->Allocate(len, 0);
		glGetProgramInfoLog(program, len, &chWritten, log);
		cout << "ProgramInfoLog:" << log << endl;
		Allocator::GetAllocator()->Deallocate((byte*)log, 0);
	}
}

//打印着色器LOG
void printShaderLog(GLuint shader)
{
	int len = 0;
	int chWritten = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)Allocator::GetAllocator()->Allocate(len, 0);
		glGetShaderInfoLog(shader, len, &chWritten, log);
		cout << "Shader info Log:" << log << endl;
		Allocator::GetAllocator()->Deallocate((byte*)log, 0);
	}
}


//读取着色器源码
string readShaderSource(const char* path)
{
	string content;
	ifstream fileStream(path, ios::in);
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();

	return content;
}

/// --------------------------------- Shader ---------------------------------------

Shader::Shader(ShaderType type)
{
	shaderType = type;
	isCompiled = false;
	handle = 0;
}

void Shader::Compile()
{
	//创建glShader  
	switch (this->shaderType)
	{
		case ShaderType::VertShader:
			this->handle = glCreateShader(GL_VERTEX_SHADER);
		break;
		case ShaderType::FragShader:
			this->handle = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		default:break;
	}

	//编译错误捕捉
	GLint isCompiled;

	//载入源码
	const char* source = content.c_str();
	glShaderSource(handle, 1, &(source), NULL);
	
	//编译
	glCompileShader(handle);
	checkOpenGLError();
	glGetShaderiv(handle, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled != 1)
	{
		switch (this->shaderType)
		{
		case ShaderType::VertShader:
			cout << "Vertex Compile Failed!" << endl;
			break;
		case ShaderType::FragShader:
			cout << "Frag Compile Failed!" << endl;
			break;
		default:break;
		}


		printShaderLog(handle);
		return;
	}

	this->isCompiled = true;
}

 
// --------------------------------- Pass ---------------------------------------

ShaderPass::ShaderPass()
{
	isCompiled = false;
	renderProgram = 0;
}

void ShaderPass::Compile()
{
	//声明错误信息
	GLint isLinked;

	//编译所有着色器  
	for (Shader* s : shaders)
	{
		s->Compile();
	}

	// -----------------------------------

	//创建渲染程序并附加着色器。然后请求GLSL编译器确保兼容性。
	GLuint program = glCreateProgram();

	//program附加着色器  
	for (Shader* s : shaders)
	{
		glAttachShader(program, s->handle);
	}

	//链接  
	glLinkProgram(program);
	checkOpenGLError();
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked != 1)
	{
		cout << "Link Failed" << endl;
		printProgramLog(program);
		return;
	}

	//完成  
	this->isCompiled = true;
	this->renderProgram = program;
}

void ShaderPass::Use()
{
	glUseProgram(this->renderProgram);
}

// --------------------------------- Material Shader ---------------------------------------

OBJECT_TYPE_IMPLEMENT(MaterialShader, Object);

MaterialShader::MaterialShader()
{
	isCompiled = false;
}

void MaterialShader::Compile()
{
	for (ShaderPass* p : passes)
	{
		p->Compile();
	}
}


//void Shader::Compile()
//{
//	//声明错误信息
//	GLint isLinked;
//
////	//编译着色器  
////	for (Pass* p : passes)
////	{
////		p,Compile();
////	}
//
//	//vertShader->Compile();
//	//fragShader->Compile();
//
//	// -----------------------------------
//
//	//创建渲染程序并附加着色器。然后请求GLSL编译器确保兼容性。
//	GLuint program = glCreateProgram();
//
//
//	glAttachShader(program, vertShader->handle);
//	glAttachShader(program, fragShader->handle);
//	
//	glLinkProgram(program);
//	checkOpenGLError();
//	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
//	if (isLinked != 1)
//	{
//		cout << "Link Failed" << endl;
//		printProgramLog(program);
//		return;
//	}
//
//	//FINISH
//	isCompiled = true;
//	this->renderProgram = program;
//}
//
//Shader::Shader()
//{
//	isCompiled = false;
//	renderProgram = 0;
//	vertShader = nullptr;
//	fragShader = nullptr;
//}
//
//void Shader::Use()
//{
//	glUseProgram(this->renderProgram);
//}
//
//ShaderProgram::ShaderProgram()
//{
//	isCompiled = false;
//	handle = 0;
//}
//
//void ShaderProgram::Compile()
//{
//	this->handle = glCreateShader(GL_VERTEX_SHADER);
//
//	//编译错误捕捉
//	GLint isCompiled;
//
//	//载入源码
//	const char* source = content.c_str();
//	glShaderSource(handle, 1, &(source), NULL);
//	
//	//编译
//	glCompileShader(handle);
//	checkOpenGLError();
//	glGetShaderiv(handle, GL_COMPILE_STATUS, &isCompiled);
//	if (isCompiled != 1)
//	{
//		cout << "Vertex Compile Failed!" << endl;
//		printShaderLog(handle);
//		return;
//	}
//	isCompiled = true;
//}
//
//FragShader::FragShader()
//{
//	isCompiled = false;
//	handle = 0;
//}
//
//void FragShader::Compile()
//{
//	this->handle = glCreateShader(GL_FRAGMENT_SHADER);
//
//	//编译错误捕捉
//	GLint isCompiled;
//
//	//载入源码
//	const char* source = content.c_str();
//	glShaderSource(handle, 1, &(source), NULL);
//
//	//编译
//	glCompileShader(handle);
//	checkOpenGLError();
//	glGetShaderiv(handle, GL_COMPILE_STATUS, &isCompiled);
//	if (isCompiled != 1)
//	{
//		cout << "Frag Compile Failed!" << endl;
//		printShaderLog(handle);
//		return;
//	}
//	isCompiled = true;
//}
