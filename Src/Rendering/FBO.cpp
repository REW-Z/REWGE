
#include "REWGE.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>


#include <cmath>
#include <stack>
#include <unordered_map>
#include <vector>
#include <list>
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include "REWCPPL.h"
#include "Utils.h"

#include "Memory/Allocator.h"

#include "FBO.h"


// ------------ FrameBuffer ----------------


FBO::FBO()
{
	fbo = 0;
}

FBO::~FBO()
{
	glDeleteFramebuffers(1, &fbo);
}


void FBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}


FBO* FBO::Default()
{
	static FBO* dftFBO = nullptr;

	if (dftFBO == nullptr)
	{
		dftFBO = NEW(FBO)FBO();
		dftFBO->fbo = 0;
		////glGenFramebuffers(1, &dftFBO->fbo);//屏幕帧缓冲不需要手动生成    
		std::cout << dftFBO->fbo << std::endl;
	}

	return dftFBO;
}
FBO* FBO::Create()
{
	FBO* frameBuffer = NEW(FBO)FBO();

	glGenFramebuffers(1, &frameBuffer->fbo);

	return frameBuffer;
}


void FBO::CurrentBindTextrue(GLuint texture, int level)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, level);
}
bool FBO::CurrentIsComplate()
{
	return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}


