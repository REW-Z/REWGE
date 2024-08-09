
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
#include <regex>
#include <sstream>
#include <vector>
#include <list>
#include <unordered_map>
#include <cmath>
#include <stack>
#include <functional>

#include "REWCPPL.h"


#include "Memory/Allocator.h"
//#include "Memory//StackAllocator.hpp"

#include "Rendering/FBO.h"

#include "Utils.h"

#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/World.h"


#include "Application.h"

#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"
#include "SubSystems/Debug.h"
#include "SubSystems/Profiler.h"
#include "SubSystems/InputManager.h"
#include "SubSystems/RenderSystem.h"
#include "SubSystems/WorldManager.h"

#include "World.h"


OBJECT_TYPE_IMPLEMENT(World, Object)



bool World::AddRootGameObject(GameObject& rootGO)
{
	if (rootGO.transform->parent != nullptr)
	{
		std::cout << "要添加到World的GameObject有父节点" << std::endl;
		return false;
	}
	return AddGameObject(rootGO);
}

bool World::AddGameObject(GameObject& go)
{
	this->gameObjects.push_back(&go);


	//递归添加所有子物体到场景世界
	if (go.transform->children.size() > 0)
	{
		for (auto child : go.transform->children)
		{
			AddGameObject(*child->gameObject);
		}
	}

	return true;
}

World& World::GetCurrentWorld()
{
	return WorldManager::Instance().currentWorld;
}
