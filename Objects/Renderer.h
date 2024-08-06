#pragma once


class Renderer : public Component
{
	OBJECT_TYPE_DECLARE(Renderer)
public:
	static std::list<Renderer*>* GetRendererList();
private:
	bool enable;
	int layer;

	
	bool isInScene;//是否在场景中  
	int scenehandle;//在场景renderers列表中的idx  

public:
	std::vector<Material*> materials;

protected:

	bool isBoundsDirty;

	glm::mat4x4		worldMatrix;
	rew::AABB		localAABB;
	rew::AABB		worldAABB;


public:
	Renderer();

	virtual void Init() override;
public:
	//渲染场景相关
	bool IsInScene();
	bool ShouldInScene();

	void SetIsInScene(bool value, int newhandle);
	void SetEnable(bool value);
	void SetLayer(int newlayer) { this->layer = newlayer; };

	int GetNodeIdx() { return scenehandle; }


	//变换和AABB相关
	void SetDirty() { isBoundsDirty = true; };
	rew::AABB GetWorldAABB();
	virtual void UpdateAABB() {};



	//DrawCall  
	virtual void Draw();
	virtual void DrawShadow();
};


class MeshRenderer : public Renderer
{
	OBJECT_TYPE_DECLARE(MeshRenderer)
public:
	Mesh* mesh;
	bool castShadow;

public:
	MeshRenderer():Renderer() {};

	virtual void UpdateAABB() ;

	virtual void Draw() override;
	virtual void DrawShadow() override;
};
