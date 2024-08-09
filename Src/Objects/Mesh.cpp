
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
#include "Objects/Texture.h"
#include "Objects/Material.h"


#include "Mesh.h"


using namespace std;
using namespace glm;


IndexlessMesh::IndexlessMesh()
{
	bufferId = -1;
	numVertices = 0;
}




//------------------------------------------------------------------------------
OBJECT_TYPE_IMPLEMENT(Mesh, Component)

Mesh::Mesh()
{
	numVertices = 0;
	binding_vbo = false;
}

void Mesh::SetupAPI()
{
	if (binding_vbo) return;

	assert(this);
	assert(this->vertices.size() > 0);
	assert(this->vertices.size() == this->numVertices);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(4, this->vbo); //4个vbo
	glGenBuffers(1, &this->ebo); //1个ebo(索引数组)

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, (unsigned int)(this->vertices.size() * 3 * 4), (float*)&this->vertices[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, (unsigned int)(this->texcoords.size() * 2 * 4), (float*)&this->texcoords[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	glBufferData(GL_ARRAY_BUFFER, (unsigned int)(this->normals.size() * 3 * 4), (float*)&this->normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[3]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
	glBufferData(GL_ARRAY_BUFFER, (unsigned int)(this->tangents.size() * 3 * 4), (float*)&this->tangents[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)(this->inds.size() * 4), &this->inds[0], GL_STATIC_DRAW);


	this->binding_vbo = true;
}

rew::AABB Mesh::CalculateAABB()
{
	rew::AABB6f aabb6;
	aabb6.min_x = 999999.0F;
	aabb6.min_y = 999999.0F;
	aabb6.min_z = 999999.0F;
	aabb6.max_x = -999999.0F;
	aabb6.max_y = -999999.0F;
	aabb6.max_z = -999999.0F;


	for (size_t i = 0; i < vertices.size(); ++i)
	{
		aabb6.ExtendTo(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	return rew::AABB(aabb6);
}


//--------------------------- Functions ----------------------------------------

glm::vec3 GetTangent(vec3 pos1, vec3 pos2, vec3 pos3, vec2 uv1, vec2 uv2, vec2 uv3, vec3 normal)
{
	glm::vec3 tangent;

	vec3 edge1 = pos2 - pos1;
	vec3 edge2 = pos3 - pos1;
	vec2 deltaUV1 = uv2 - uv1;
	vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = normalize(tangent);

	//glm::vec3 bitangent;

	//bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	//bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	//bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	//bitangent = normalize(bitangent);

	return tangent;
}