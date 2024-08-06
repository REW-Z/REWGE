
#include "REWGE.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>


#include <cmath>
#include <stack>
#include <unordered_map>
#include <vector>
#include <list>
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include "REWCPPL.h"
#include "Utils.h"

#include "Memory/Allocator.h"

#include "Rendering/FBO.h"

#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"

#include "Objects/Light.h"
#include "Objects/Texture.h"
#include "Objects/Shader.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/Renderer.h"
#include "Objects/World.h"

#include "SubSystems/RenderSystem.h"

#include "Camera.h"




// --------------------------------- CAMERA ------------------------------------

OBJECT_TYPE_IMPLEMENT(Camera, Behaviour)

Camera::Camera()
{
	_near = 0.1f;
	_far = 1000.0f;
	fov = 60.0f;
	aspectRatio = 1.0f;

	vMatDirty = true;
	pMatDirty = true; 

	p_mat_tmp = glm::mat4();
	v_mat_tmp = glm::mat4();

	bgColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void Camera::Init()
{
	assert(gameObject);
	gameObject->onTransformChange.Add(this, std::bind(&Camera::SetCameraDirty, this));
}


void Camera::SetCameraDirty()
{
	vMatDirty = true;
	pMatDirty = true;
}

//V视图变换矩阵
glm::mat4 Camera::GetMatrixV()
{
	if (vMatDirty)
	{
		//// *** 自定义观察空间矩阵 *** (Valid)
		//glm::vec3 pos = gameObject->transform->worldPosition();
		//glm::vec3 gaze = gameObject->transform->forward();
		//glm::vec3 up = gameObject->transform->up();
		//glm::vec3 right = glm::cross(gaze, up);

		//glm::mat4 mTview = glm::transpose(glm::mat4(
		//	1.0f, 0.0f, 0.0f, -pos.x,
		//	0.0f, 1.0f, 0.0f, -pos.y,
		//	0.0f, 0.0f, 1.0f, -pos.z,
		//	0.0f, 0.0f, 0.0f, 1.0f
		//));
		//glm::mat4 mRview = glm::transpose(glm::mat4( //基构建
		//	right.x, right.y, right.z, 0.0f,
		//	up.x, up.y, up.z, 0.0f,
		//	-gaze.x, -gaze.y, -gaze.z, 0.0f, //gaze和正Z是反向的
		//	0.0f, 0.0f, 0.0f, 1.0f
		//));
		//this->v_mat_tmp = mRview * mTview;


		// *** 直接用相机(模型 -> 世界)的逆矩阵 ***  (Valid) 
		glm::mat4 worldToCam = glm::inverse(this->gameObject->transform->GetLocalToWorldMat());
		this->v_mat_tmp = worldToCam;


		//  *** 矩阵脏标记去除  ***
		vMatDirty = false;
	}


	return v_mat_tmp;
}



//P投影变换矩阵
glm::mat4 Camera::GetMatrixP()
{
	if (pMatDirty)
	{

		// ****** Use Custom ****** (Valid) 
		float fovRad = (glm::pi<float>() / 180.0f) * fov;

		p_mat_tmp =  transpose(glm::mat4(
			(1.0f / tan(fovRad / 2.0f)) / aspectRatio, 0.0f, 0.0f, 0.0f,
			0.0f, (1.0f / tan(fovRad / 2.0f)), 0.0f, 0.0f,
			0.0f, 0.0f, (_near + _far) / (_near - _far), (2.0f * _near * _far) / (_near - _far),
			0.0f, 0.0f, -1.0f, 0.0f
		));


		// ****** USE GLM ******  (Valid) 
		//float fovRad = (glm::pi<float>() / 180.0f) * fov;
		//p_mat_tmp = glm::perspective(fovRad, aspectRatio, _near,_far);



		//  *** 矩阵脏标记去除  ***
		pMatDirty = false;
	}
	return p_mat_tmp;
}

glm::mat4 Camera::GetMatrixVP()
{
	return GetMatrixP() * GetMatrixV();
}

void Camera::CalculateFrustumPlanes()
{
	glm::vec4 planes[6];

	//// 提取平面
	glm::mat4 VP = GetMatrixP() * GetMatrixV(); // 确保顺序为投影 * 视图
	VP = glm::transpose(VP);
	// Left
	planes[0] = VP[3] + VP[0];
	// Right
	planes[1] = VP[3] - VP[0];
	// Bottom
	planes[2] = VP[3] + VP[1];
	// Top
	planes[3] = VP[3] - VP[1];
	// Near
	planes[4] = VP[3] + VP[2];
	// Far
	planes[5] = VP[3] - VP[2];


	//// 提取平面
	//glm::mat4 VP = GetMatrixP() * GetMatrixV(); // 确保顺序为投影 * 视图
	//// 左平面
	//planes[0] = glm::vec4(
	//	VP[0][3] + VP[0][0],
	//	VP[1][3] + VP[1][0],
	//	VP[2][3] + VP[2][0],
	//	VP[3][3] + VP[3][0]);
	//// 右平面
	//planes[1] = glm::vec4(
	//	VP[0][3] - VP[0][0], 
	//	VP[1][3] - VP[1][0],
	//	VP[2][3] - VP[2][0],
	//	VP[3][3] - VP[3][0]);
	//// 底平面
	//planes[2] = glm::vec4(
	//	VP[0][3] + VP[0][1], 
	//	VP[1][3] + VP[1][1],
	//	VP[2][3] + VP[2][1], 
	//	VP[3][3] + VP[3][1]);
	//// 顶平面
	//planes[3] = glm::vec4(
	//	VP[0][3] - VP[0][1],
	//	VP[1][3] - VP[1][1],
	//	VP[2][3] - VP[2][1],
	//	VP[3][3] - VP[3][1]);
	//// 近平面
	//planes[4] = glm::vec4(
	//	VP[0][3] + VP[0][2],
	//	VP[1][3] + VP[1][2],
	//	VP[2][3] + VP[2][2],
	//	VP[3][3] + VP[3][2]);
	//// 远平面
	//planes[5] = glm::vec4(
	//	VP[0][3] - VP[0][2],
	//	VP[1][3] - VP[1][2],
	//	VP[2][3] - VP[2][2],
	//	VP[3][3] - VP[3][2]);


	for (int i = 0; i < 6; i++) {
		glm::vec3 normal = glm::vec3(planes[i]); // 平面的法向量部分
		float length = glm::length(normal); // 计算法向量长度
		normal /= length; // 规范化法向量
		float distance = planes[i].w / length; // 计算距离

		frustumPlanes[i].normal = RewVec3(normal);
		frustumPlanes[i].distance = distance;
	}
}



bool Camera::IsInFrustum(rew::AABB& aabb)
{
	CalculateFrustumPlanes();

	for (int i = 0; i < 6; i++) {
		rew::Vector3 normal = frustumPlanes[i].normal;
		float distance = frustumPlanes[i].distance;

		rew::Vector3 positiveVertex = aabb.center + rew::Vector3(
			normal.x > 0 ? aabb.half_length.x : -aabb.half_length.x,
			normal.y > 0 ? aabb.half_length.y : -aabb.half_length.y,
			normal.z > 0 ? aabb.half_length.z : -aabb.half_length.z
		);

		if (rew::Vector3::Dot(normal, positiveVertex) + distance < 0) {
			return false;
		}
	}
	return true;
}



