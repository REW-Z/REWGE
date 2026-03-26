#pragma once

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
#include <map>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>

#include "REWCPPL.h"
#include "Utils.h"

#include "Memory/Allocator.h"

#include "Rendering/FBO.h"

#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Shader.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/Renderer.h"
#include "Objects/World.h"  


#include "SubSystems/Debug.h"
#include "SubSystems/FileSystem.h"
#include "SubSystems/AssetPipeline.h"
#include "SubSystems/ResourceManager.h"





// ----------------------------------- Resource Mgr ------------------------------------------

//资源注册表
std::unordered_map<std::string, Object*> ResourceManager::loadedResources;

//载入资源  
Object* ResourceManager::Load(TypeEnum type, std::string path)
{
	Object* asset = nullptr;
	bool load_successful = false;

	//Debug::Log("尝试载入资源- 资源路径：");
	//Debug::Log(path);

	//Try Find In Loaded Assets
	std::unordered_map<std::string, Object*>::const_iterator it = loadedResources.find(path);
	if (it != loadedResources.end())
	{
		//Debug::Log("已经找到已载入资源- 资源名：");
		//Debug::WriteLogFile();
		//Debug::Log(path);
		return reinterpret_cast<Object*>(it->second);
	}
	else
	{
		//Debug::Log("未找到已载入资源，载入中");
		//Debug::WriteLogFile();
	}


	switch (type)
	{
		// IS TEXTRUE2D
		case TypeEnum::TYPE_Texture2D:
		{
			Texture2D* importedTex = AssetPipeline::Import_IMG(path);
			asset = importedTex as(Object);
		}
		break;
		case TypeEnum::TYPE_MaterialShader:
		{
			MaterialShader* importedShader = AssetPipeline::Import_GLSL(path);
			asset = importedShader as (Object);
		}
		break;
		default:
		{
			//按后缀名过滤示例
			//if (Path::GetExtension(path) == ".obj")
			//{
			//	//...
			//}
			//else
			//{
			//	Debug::Log("未支持该格式的模型！" + path);
			//}

			Debug::Log("未实现该类型载入的实现！！");
			return nullptr;
		}
		break;
	}


	if (asset != nullptr)
	{
		load_successful = true;
	}


	//添加到资源表
	if (load_successful)
	{
		auto item = std::pair<std::string, Object*>(path, dynamic_cast<Object*>(asset));
		ResourceManager::loadedResources.insert(item);
	}
	std::cout << "LoadEnd: " << path << std::endl;
	return asset;
}

//卸载资源  
bool ResourceManager::Unload(TypeEnum type, std::string path)
{
	Object* objToUnload = nullptr;
	std::unordered_map<std::string, Object*>::const_iterator it = loadedResources.find(path);
	if (it != loadedResources.end())
	{
		Object* obj = (*it).second;

		loadedResources.erase(it);

		Object::DeleteInstance(obj);

		return true;
	}
	return false;
}



//--------------------------------------------------------------------------------------------
