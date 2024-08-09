#pragma once

using namespace std;

// -------- Other ---------------

float toRadians(float degrees);
void PrintMatrix(glm::mat4 mat);

// ----------- Geometry -----------

rew::Vector3 RewVec3(glm::vec3& vec);
rew::Vector3 RewVec3(glm::vec3&& vec);
rew::Vector3* AsRewVec3(glm::vec3& vec);

glm::vec3 GlmVec3(rew::Vector3& vec);
glm::vec3* AsGlmVec3(rew::Vector3& vec);