
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


#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"


#include "Material.h"



OBJECT_TYPE_IMPLEMENT(Material, Object)

Material::Material()
{
	this->textureBump = Texture2D::emptyTexture;
	this->textureBump = Texture2D::emptyTexture;
}