
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

#include "WorldManager.h"






WorldManager& WorldManager::Instance()
{
	static WorldManager* mgr = new WorldManager();
	return *mgr;
}
