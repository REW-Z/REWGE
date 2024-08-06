#pragma once

#include "Rendering/FBO.h"


class RenderTexture : public Object
{
	OBJECT_TYPE_DECLARE(RenderTexture)
public:
	FBO* fbo;
	GLuint tex;
public:
	int width;
	int height;
public:
	RenderTexture();
	RenderTexture(int height, int width);
	void Reset() {};//ÖØÉèº¯Êı£¨ÏñËØClear?£©

//static  
public:
	static RenderTexture* GetTemp(int width, int height);
	static void ReleaseTemp(RenderTexture* rt);
};


class RenderTextureManager
{
public:
	std::unordered_map<int, std::vector<RenderTexture*>> freeTextures;
	std::unordered_map<int, std::vector<RenderTexture*>> takenTextures;

public:
	RenderTexture* GetTempRenderTexture(int width, int height);
	void ReleaseTempRenderTexture(RenderTexture* rt);
//static  
public:
	static RenderTextureManager* Instance();
};

