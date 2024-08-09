
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
#include "Objects/Mesh.h"
#include "Objects/Camera.h"
#include "Objects/Texture.h"

#include "Objects/World.h"

#include "Application.h"

#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"
#include "SubSystems/RenderSystem.h"
#include "SubSystems/Debug.h"
#include "SubSystems/Profiler.h"

#include "Light.h"  


// ----------------------------------- Light ----------------------------------------------
OBJECT_TYPE_IMPLEMENT(Light, Behaviour)

Light::Light()
{
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//SetupShadowMap();
}

glm::vec3 Light::dir()
{
	return gameObject->transform->forward();
}

//void Light::SetupShadowMap()
//{
//	glGenFramebuffers(1, &shadowBuffer);
//
//
//	glGenTextures(1, &shadowMap);
//	glBindTexture(GL_TEXTURE_2D, shadowMap);
//
//	int width = RenderSystem::Instance()->frame_buffer_width;
//	int height = RenderSystem::Instance()->frame_buffer_height;
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//
//	// may reduce shadow border artifacts
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//}
