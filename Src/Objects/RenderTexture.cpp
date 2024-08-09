
#include "REWGE.h"

#include <ctime>
#include <typeinfo>
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

#include "Rendering/FBO.h"

#include "Objects/Object.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"

#include "RenderTexture.h"


OBJECT_TYPE_IMPLEMENT(RenderTexture, Object)

RenderTexture::RenderTexture()
{
	width = 0;
	height = 0;
}
RenderTexture::RenderTexture(int width, int height)
{
	//fbo
	fbo = FBO::Create();

	//tex
	glGenTextures(1, &this->tex);
	glBindTexture(GL_TEXTURE_2D, this->tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// may reduce shadow border artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	this->width = width;
	this->height = height;
}

RenderTexture* RenderTexture::GetTemp(int width, int height)
{
	return RenderTextureManager::Instance()->GetTempRenderTexture(width, height);
}
void RenderTexture::ReleaseTemp(RenderTexture* rt)
{
	RenderTextureManager::Instance()->ReleaseTempRenderTexture(rt);
}

/// ***************************************************************************************************

int ResolutionHash(int width, int height)
{
	auto hash1 = std::hash<int>{}((width));
	auto hash2 = std::hash<int>{}((height));
	return hash1 ^ hash2;
}

RenderTextureManager* RenderTextureManager::Instance()
{
	static RenderTextureManager mgr;
	return &mgr;
}
RenderTexture* RenderTextureManager::GetTempRenderTexture(int width, int height)
{
	auto key = ResolutionHash(width, height);
	
	if (freeTextures[key].size() > 0)
	{
		auto rt = freeTextures[key].back();
		freeTextures[key].pop_back();
		takenTextures[key].push_back(rt);
		rt->Reset();
		return rt;
	}
	else
	{
		RenderTexture* rtNew = NEW(RenderTexture)RenderTexture(width, height);
		takenTextures[key].push_back(rtNew);
		rtNew->Reset();
		return rtNew;
	}
}

void RenderTextureManager::ReleaseTempRenderTexture(RenderTexture* rt)
{
	auto key = ResolutionHash(rt->width, rt->height);

	auto& takenList = takenTextures[key];
	auto it = std::find(takenList.begin(), takenList.end(), rt);
	if (it != takenList.end()) {
		takenList.erase(it);
		freeTextures[key].push_back(rt);
	}
}

