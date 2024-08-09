
#include "REWGE.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cstdarg>
#include <cmath>

#include <stack>
#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <functional>
#include "REWCPPL.h"

#include "Memory/Allocator.h"

#include "Objects/Object.h"
#include "Objects/BaseComponents.h"
#include "Objects/Mesh.h"
#include "Objects/World.h"

#include "SubSystems/WorldManager.h"
#include "SubSystems/ResourceManager.h"


#include "Objects/GameObject.h"

// --------------------------------------------- GameObject -------------------------------------------

OBJECT_TYPE_IMPLEMENT(GameObject, Object)

GameObject::GameObject()
{
	this->transform = nullptr;
	this->AddComponent(TypeEnum::TYPE_Transform);
}

GameObject::~GameObject()
{
	//delete transform;
}

Component * GameObject::GetComponent(TypeEnum type)
{
	Component * componentPtr = nullptr;
	for (std::vector<Component *>::iterator it = components.begin(); it != components.end(); (it)++)
	{
		if ((*it)->GetTypeEnum() == type)
		{
			componentPtr = *it;
		}
	}
	return componentPtr;
}

Component* GameObject::AddComponentInternal(Component * c)
{
	components.push_back(c);
	
	//Component的gameObject指针
	c->gameObject = this;

	//GameObject的组件指针
	switch (c->GetTypeEnum())
	{
	case TypeEnum::TYPE_Transform:
		this->transform = dynamic_cast<Transform*>(c);
		break;
	default:
		break;
	}

	//init component
	c->Init();

	return c;
}

Component* GameObject::AddComponent(TypeEnum type)
{
	Component* newcom = dynamic_cast<Component*>( Object::CreateInstance(type));
	AddComponentInternal(newcom);
	return newcom;
}


// --------------------------------------------- GameObjectUtility -------------------------------------------


GameObject& GameObjectUtility::CreateGameObject(std::string name , ...)
{
	GameObject* go = dynamic_cast<GameObject*> (Object::CreateInstance(TypeEnum::TYPE_GameObject) );

	//set name
	go->name = name;

	//add tranform com (构造时就已经添加Transform)
	//if(go->GetComponent(TypeEnum::TYPE_Transform) == nullPtr) go->AddComponent(TypeEnum::TYPE_Transform);
	

	//初始组件
	va_list args;
	va_start(args, name);
	for (int i = 0; i < 99; i++)
	{
		TypeEnum arg = va_arg(args, TypeEnum);

		if (arg == TypeEnum::TYPE_Transform) continue;
		if (arg == TypeEnum::TYPE_UNDEFINED) break;

		auto type = (*Object::GetTypeMap())[arg];

		if (type && type->Is(TypeEnum::TYPE_Component))
		{
			go->AddComponent(arg);
		}
	}


	return *go;
}