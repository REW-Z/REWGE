#pragma once

#include "REWGE.h"

#include <ctime>
#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <fstream>


#include <iostream>
#include <string>
#include <sstream>
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
#include "Objects/Material.h"
#include "Objects/Mesh.h"


#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"

#include "Objects/Texture.h"

// -----------------------------------------------------------

OBJECT_TYPE_IMPLEMENT(Texture2D, Object)

Texture2D* Texture2D::emptyTexture = nullptr; //����Ⱦϵͳ��ʼ��ʱ��ֵ����ʱglInit()֮ǰ�޷���ȡ���� 

void Texture2D::Init()
{
	// MipMap
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//�������Թ���
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
	}
	//ClampOrRepeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture2D::SaveAs(std::string path, int imageType) //SOIL_SAVE_TYPE_PNG as Default
{
	cout << "�����ļ���" << path << "��" << "Resources/Output/...." << endl;
	SOIL_save_image(path.c_str(), imageType, width, height, channels, (unsigned char*)rawData);
	cout << "�������" << endl;
}

Texture2D::~Texture2D()
{
	//ɾ��OGL���
	glDeleteTextures(1, &handle);

	//ɾ��SOILԭʼ����
	if (rawData != nullptr)
	{
		SOIL_free_image_data((unsigned char*)rawData);
		//Allocator::GetInstance()->Deallocate((byte*)dataBuffer, 0);
	}
}



