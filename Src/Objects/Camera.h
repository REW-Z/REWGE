#pragma once



class Frustum
{
public:
	rew::Plane frustumPlanes[6];
public:
	Frustum();
	void UpdateFrustum(glm::mat4 matVP);
	bool IsInFrustum(rew::AABB& aabb);
};


class Camera: public Behaviour
{
	OBJECT_TYPE_DECLARE(Camera)

public:
	FBO* renderTarget;

public:

	glm::mat4 v_mat_tmp;
	glm::mat4 p_mat_tmp;
	Frustum frustum;

	bool vMatDirty;
	bool pMatDirty;
	bool frustumDirty;
public:
	Camera();
	virtual void Init() override;

	//glm::vec3 e;//position  £¨translate To 0,0,0£©
	//glm::vec3 g;//forward  £¨rotate To -z£©
	//glm::vec3 t;//up  £¨rotate To +y£©

	float _near; // near = 1 / tan(fov / 2)
	float _far;
	float aspectRatio;
	float fov;


	void SetCameraDirty();

	glm::mat4 GetMatrixV();
	glm::mat4 GetMatrixP();
	glm::mat4 GetMatrixVP();

	//IsInFrustum
	Frustum* GetFrustum();
	void UpdateFrustum();
	bool IsInFrustum(rew::AABB& aabb);


public:

	//render settings
	bool cleanColorBuffer;
	bool cleanDepthBuffer;
	glm::vec4 bgColor;
};


