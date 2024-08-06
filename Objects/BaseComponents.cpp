#pragma once


#include "REWGE.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>



#include <cmath>
#include <stack>
#include <vector>
#include <list>
#include <unordered_map>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>

#include "REWCPPL.h"

#include "Memory/Allocator.h"
//#include "Memory//StackAllocator.hpp"


#include "Object.h"
#include "GameObject.h"
#include "BaseComponents.h"
#include "Mesh.h"

#include "SubSystems/ResourceManager.h"

//using namespace rew;
using namespace glm;

// -------------------------------------- Component ---------------------------------------
OBJECT_TYPE_IMPLEMENT(Component, Object)
Component::Component() { this->gameObject = nullptr; }


// -------------------------------------- Transform ---------------------------------------
OBJECT_TYPE_IMPLEMENT(Transform, Component)
Transform::Transform()
{
	local_position = glm::vec3(0.0f, 0.0f, 0.0f);
	local_rotation = glm::quat_cast(glm::mat4(1.0f));
	local_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	parent = nullptr;

	//m矩阵
	m_mat_tmp = glm::mat4();
	isTransformDirty = true;
}

void Transform::Init()
{
}

// -------------------------------- Static Fields  -------------------------------
float Transform::rad2deg = 180.0f / pi<float>();
float Transform::deg2rad = pi<float>() / 180.0f;



// -------------------------------- LOCAL GET   -------------------------------

glm::vec3 Transform::localPosition()
{
	return local_position;
}
glm::quat Transform::localRotation()
{
	return local_rotation;
}
glm::vec3 Transform::localScale()
{
	return local_scale;
}
glm::vec3 Transform::localEularAngles()
{
	return glm::eulerAngles(this->local_rotation) * rad2deg;
}




// -------------------------------- LOCAL SET   -------------------------------

void Transform::localPosition(float x, float y, float z)
{
	this->local_position = glm::vec3(x, y, z);
	SetTransformDirty();
}
void Transform::localRotation(float x, float y, float z, float w)
{
	this->local_rotation = glm::quat(w, x, y, z); //警告：quat构造函数中w的位置在第一个！！
	SetTransformDirty();
}
void Transform::localScale(float x, float y, float z)
{
	this->local_scale = glm::vec3(x, y, z);
	SetTransformDirty();
}
void Transform::localEularAngles(float x, float y, float z)
{
	glm::mat4 matrixEular = UtilsGetMatrixFromEular(x, y, z, RotateOrder::XYZ);
	this->local_rotation = glm::quat_cast(matrixEular);

	SetTransformDirty();
}

void Transform::localPosition(glm::vec3 v)
{
	this->local_position = v;
	SetTransformDirty();
}

void Transform::localRotation(glm::quat q)
{
	this->local_rotation = q; //警告：quat构造函数中w的位置在第一个！！
	SetTransformDirty();
}

void Transform::localScale(glm::vec3 v)
{
	this->local_scale = v;
	SetTransformDirty();
}

void Transform::localEularAngles(glm::vec3 v)
{
	glm::mat4 matrixEular = UtilsGetMatrixFromEular(v.x, v.y, v.z, RotateOrder::XYZ);
	this->local_rotation = glm::quat_cast(matrixEular);

	SetTransformDirty();
}



// -------------------------------- WORLD  GET    -------------------------------

glm::vec3 Transform::worldPosition()
{
	if (this->parent)
	{
		return glm::vec3(parent->GetLocalToWorldMat() * glm::vec4(this->local_position, 1.0f));
	}
	else
	{
		return this->local_position;
	}
}

glm::quat Transform::worldRotation()
{
	glm::quat worldRot = this->local_rotation;
	Transform* current = this->parent;
	while (current)
	{
		worldRot = current->local_rotation * worldRot;
		current = current->parent;
	}

	return worldRot;
}

glm::vec3 Transform::worldEularAngles()
{
	return glm::eulerAngles(this->worldRotation()) * rad2deg;
}

glm::mat4 Transform::worldScale()
{
	glm::mat4 invRot;
	invRot = glm::mat4_cast(glm::inverse(worldRotation()));
	glm::mat4 scaleAndRot = worldRotationAndScale();

	return invRot * scaleAndRot;
}


glm::mat4 Transform::worldRotationAndScale()
{
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), local_scale);

	glm::mat4 rotation = glm::mat4_cast(local_rotation);

	if (this->parent)
	{
		return (this->parent->worldRotationAndScale()) * rotation * scale;
	}
	else
	{
		return rotation * scale;
	}
}

// -------------------------------- WORLD  SET    -------------------------------

void Transform::worldRotation(glm::quat q)
{
	if (parent)
	{
		localRotation( glm::inverse(parent->worldRotation()) * q );
	}
	else
	{
		localRotation(q);
	}
}







// ----------------- Directions ---------------------------



glm::vec3 Transform::up()
{
	return GetLocalToWorldMat() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::forward() 
{
	return GetLocalToWorldMat() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
}

glm::vec3 Transform::right()
{
	return GetLocalToWorldMat() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
}

void Transform::Rotate(float x, float y, float z, Space space = Space::Local)
{
	if (space == Space::Local)
	{
		auto q = glm::quat_cast(UtilsGetMatrixFromEular(x, y, z, RotateOrder::XYZ));
		this->localRotation(q * this->localRotation());
	}
	else
	{
		auto q = glm::quat_cast(UtilsGetMatrixFromEular(x, y, z, RotateOrder::XYZ));
		this->worldRotation(q * this->worldRotation());
	}
}

void Transform::RotateAxisAngle(glm::vec3 axis, float angleDeg, Space space = Space::World)
{
	if (space == Space::Local) //Valid
	{
		auto q = glm::rotate(this->localRotation(), angleDeg * deg2rad, axis);

		this->localRotation(q);
	}
	else//Valid
	{
		vec3 localAxis = glm::inverse(worldRotation()) * axis;

		auto q = glm::rotate(this->worldRotation(), angleDeg * deg2rad, localAxis);

		this->worldRotation(q);
	}
}



// ----------------- Matrix & Dirty ---------------------------

mat4 Transform::GetLocalToWorldMat()
{
	if (isTransformDirty)
	{
		mat4 mS = glm::scale(mat4(1.0f), vec3(local_scale.x, local_scale.y, local_scale.z));
		//mat4 mR = UtilsGetMatrixFromEular(localEularAngles().x, localEularAngles().y, localEularAngles().z, RotateOrder::XYZ);		  // ** valid **
		mat4 mR = glm::mat4_cast(this->localRotation());																				// ** valid **
		mat4 mT = glm::translate(mat4(1.0f), vec3(local_position.x, local_position.y, local_position.z));


		mat4 resultThis = (mT * mR * mS); // (mT * mRy * mRx * mRz * mS);


		if (parent != nullptr)
			this->m_mat_tmp = (parent->GetLocalToWorldMat()) * resultThis;
		else
			this->m_mat_tmp = resultThis;
	}

	return m_mat_tmp;
}

void Transform::SetTransformDirty()
{
	isTransformDirty = true;

	//Childs
	std::vector<Transform*>::iterator end = children.end();
	for (std::vector<Transform*>::iterator it = children.begin(); it != end; ++it)
	{
		(*it)->SetTransformDirty();
	}


	//GameObject Message: Transform Changed
	assert(gameObject);
	gameObject->onTransformChange.Invoke();
}



// ----------------- UTILS ---------------------------
glm::mat4 Transform::UtilsGetMatrixFromEular(float x, float y, float z, RotateOrder order)
{
	mat4 mRz = glm::rotate(mat4(1.0f), (z * deg2rad), vec3(0.0f, 0.0f, 1.0f));
	mat4 mRx = glm::rotate(mat4(1.0f), (x * deg2rad), vec3(1.0f, 0.0f, 0.0f));
	mat4 mRy = glm::rotate(mat4(1.0f), (y * deg2rad), vec3(0.0f, 1.0f, 0.0f));
	
	switch (order)
	{
	case RotateOrder::ZXY:
		return mRy * mRx * mRz;
	case RotateOrder::XYZ:
		return mRz * mRy * mRx;
	default:
		return mRz * mRy * mRx;
	}
}





// -------------------------------------- Behaviour ---------------------------------------
OBJECT_TYPE_IMPLEMENT(Behaviour, Component)

//
//// -------------------------------------- MonoScript ---------------------------------------
//OBJECT_TYPE_IMPLEMENT(MonoScript, Behaviour)
//
//
//MonoScript::MonoScript()
//{
//	this->objInstance = nullptr;
//}
//
//bool MonoScript::Init(const char * name, MonoObject * instance, MonoDomain* domain, MonoImage* image_core, MonoImage* image_scripts)//domain //image参数
//{
//	this->objInstance = nullptr;
//
//	MonoClass * monoclassCurrentScript = mono_class_from_name(image_scripts, "MonoCSharp", name);
//	if (monoclassCurrentScript == nullptr)
//	{
//		//cout << "didn't find class in Assembly_Scripts, now query in Assembly_Engine..." << endl;
//		monoclassCurrentScript = mono_class_from_name(image_core, "MonoCSharp", name);
//	}
//	if (monoclassCurrentScript == nullptr)
//	{
//		//cout << "no class founded!!!" << endl;
//		return false;
//	}
//
//	this->objInstance = instance;
//
//	return true;
//}
