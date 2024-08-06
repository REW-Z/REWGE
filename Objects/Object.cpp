
#include "REWGE.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <random>
#include <functional>
#include <cmath>
#include <stack>
#include <regex>
#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include "REWCPPL.h"

#include "Memory/Allocator.h"

#include "Rendering/FBO.h"

#include "Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/Renderer.h"
#include "Objects/World.h"

#include "SubSystems/ResourceManager.h"




uint64_t GenerateGuid64()
{
	std::mt19937 engine(std::random_device{}());
	std::uniform_int_distribution<uint64_t> distribution;
	auto result = distribution(engine);

	return result;
}


// ------------------------------------------------------ TYPE -------------------------------------------------------

Type::Type(const char* name, TypeEnum typeEnum, Type* base, ConstructorFunction ctor)
{
	this->name = name;
	this->typeEnum = typeEnum;
	this->baseType = base;
	this->constructor = ctor;

	Object::GetTypeMap()->insert({ typeEnum, this });
}
bool Type::Is(Type* targetType)
{
	if (this == targetType) return true;

	Type* current = this;

	int count = 0;
	while (current->baseType != current)
	{
		current = current->baseType;

		if (current == targetType)
			return true;

		if (++count > 255) { std::cout << "类型错误-找不到基类!" << std::endl;  break; }
	}

	return false;
}

bool Type::Is(TypeEnum type)
{
	if (this->typeEnum == type) return true;

	Type* current = this;

	int count = 0;
	while (current->baseType != current)
	{
		current = current->baseType;

		if (current->typeEnum == type)
			return true;

		if (++count > 255) { std::cout << "类型错误-找不到基类!" << std::endl;  break; }
	}

	return false;
}



// ---------------------------------------- Object ---------------------------------------------------

OBJECT_TYPE_IMPLEMENT(Object, Object)  


/// <summary>
/// 默认构造
/// </summary>
Object::Object()
:refNum(0)
{
}

/// <summary>
/// 类型映射表
/// </summary>
std::unordered_map<int, Type*>* Object::GetTypeMap()
{
	static std::unordered_map<int, Type*> map;
	return &map;
}

/// <summary>
/// GUID到Object的映射
/// </summary>
std::unordered_map<size_t, Object*>* Object::GetIDToObjectMap()
{
	static std::unordered_map<size_t, Object*> idObjMap;
	return &idObjMap;
}

Object* Object::FindObject(size_t hash)
{
	std::unordered_map<size_t, Object*>::iterator it = GetIDToObjectMap()->find(hash);

	if (it != GetIDToObjectMap()->end())
	{
		return (it->second);
	}
	return NULL;
}

/// <summary>
/// Produce Object
/// </summary>
Object* Object::CreateInstance(TypeEnum type, std::string stringid)
{
	//get type
	auto t = (*GetTypeMap())[type];

	if (t != NULL)
	{
		Object* instance = t->constructor();

		//assign GUID
		if (stringid == "")
		{
			instance->instID = StringId("TEMP_" +  std::to_string(GenerateGuid64()));
		}
		else
		{
			instance->instID = StringId(stringid);
		}

		//insert to obj map
		InsertToObjectMap(instance->instID.gethash(), instance);

		return instance;
	}
	else
	{
		return nullptr;
	}
}

bool Object::DeleteInstance(Object* obj)
{
	if (obj != nullptr)
	{
		DEL(obj);
		return true;
	}
	return false;
}

/// <summary>
/// 插入映射表
/// </summary>
void Object::InsertToObjectMap(size_t hash, Object* obj)
{
	assert(hash != 0);

	(*GetIDToObjectMap()).insert({hash, obj});
}

/// <summary>
/// 类型判断
/// </summary>
bool Object::Is(TypeEnum type)
{
	return this->GetType()->Is(type);
}

