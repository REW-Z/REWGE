#pragma once

#include "REWGE.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "REWCPPL.h"

#include "Memory/Allocator.h"
#include "Utils.h"


#define CONCAT(a, b) (a""b)



using namespace std;



// ------------------------------------------------------- Others --------------------------------------------

float toRadians(float degrees)
{
	return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void PrintMatrix(glm::mat4 mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%f", mat[j][i]);
			if (j < 3) printf(", ");
		}
		printf("\n");
	}
}



// ------------------------------------------------------- Geometry --------------------------------------------
rew::Vector3 RewVec3(glm::vec3& vec)
{
	assert(sizeof(glm::vec3) == 12);
	assert(sizeof(rew::Vector3) == 12);


	rew::Vector3 newvec;
	memcpy(&newvec, &vec, 12);
	return newvec;
}
rew::Vector3 RewVec3(glm::vec3&& vec)
{
	assert(sizeof(glm::vec3) == 12);
	assert(sizeof(rew::Vector3) == 12);


	rew::Vector3 newvec;
	memcpy(&newvec, &vec, 12);
	return newvec;
}
rew::Vector3* AsRewVec3(glm::vec3& vec)
{
	return reinterpret_cast<rew::Vector3*>(&vec);
}

glm::vec3 GlmVec3(rew::Vector3& vec)
{
	assert(sizeof(glm::vec3) == 12);
	assert(sizeof(rew::Vector3) == 12);


	glm::vec3 newvec;
	memcpy(&newvec, &vec, 12);
	return newvec;
}

glm::vec3* AsGlmVec3(rew::Vector3& vec)
{
	return reinterpret_cast<glm::vec3*>(&vec);
}
