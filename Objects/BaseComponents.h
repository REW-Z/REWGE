#pragma once 



class Component;
class Transform;

class Component : public Object
{
	OBJECT_TYPE_DECLARE(Component)

public:
	GameObject * gameObject;

	Component();

	virtual void Init() {}
};


// ----------------------------------------------------------------------------------------------------------------------

	//quat的lookat参数必须normalize，但是mat的lookat参数不需要。  


class Transform : public Component
{
	OBJECT_TYPE_DECLARE(Transform)

private:

	glm::vec3 local_position;
	glm::quat local_rotation;
	glm::vec3 local_scale;

	glm::mat4 m_mat_tmp;
	bool isTransformDirty;
public:
	enum RotateOrder
	{
		XYZ = 0,  //glm style
		ZXY = 1,	//unity style
		YZX = 2,
	};
	enum Space
	{
		Local = 0,
		World = 1
	};
public:
	Transform();

	virtual void Init() override;

	Transform * parent;
	std::vector<Transform*> children;


	//get
	glm::vec3 localPosition();
	glm::quat localRotation();
	glm::vec3 localScale();
	glm::vec3 localEularAngles();
	
	//set
	void localPosition(float x, float y, float z);
	void localRotation(float x, float y, float z, float w);
	void localScale(float x, float y, float z);
	void localEularAngles(float x, float y, float z);
	void localPosition(glm::vec3 v);
	void localRotation(glm::quat q);
	void localScale(glm::vec3 v);
	void localEularAngles(glm::vec3 v);


	//world get
	glm::vec3 worldPosition();
	glm::quat worldRotation();
	glm::vec3 worldEularAngles();
	glm::mat4 worldScale();
	glm::mat4 worldRotationAndScale();
	//world set
	void worldRotation(glm::quat q);


	//dirs
	glm::vec3 up();
	glm::vec3 forward();
	glm::vec3 right();


	//rotate
	void Rotate(float x, float y, float z, Space space);
	void RotateAxisAngle(glm::vec3 axis, float angle, Space space);



	glm::mat4 GetLocalToWorldMat();

	void SetParent(Transform * newParent)
	{
		this->parent = newParent;
		newParent->children.push_back(this);

		SetTransformDirty();
	}
	void SetTransformDirty();


	//Transform Utility
public:
	static glm::mat4 UtilsGetMatrixFromEular(float x, float y, float z, RotateOrder order);
	static float rad2deg;
	static float deg2rad;

};







class Behaviour : public Component
{
	OBJECT_TYPE_DECLARE(Behaviour)
public:
	virtual void Init() override {};
};

//class MonoScript : public Behaviour
//{
//	OBJECT_TYPE_DECLARE(MonoScript)
//public:
//	MonoObject * objInstance;
//	MonoScript();
//	bool Init(const char * name, MonoObject * instance, MonoDomain* domain, MonoImage* image_core, MonoImage* image_scripts);
//};
//
//
//


