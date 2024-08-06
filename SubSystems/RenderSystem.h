#pragma once

//***************************************************************************************
//������ʹ����������ϵ(Y-UP) (ͬglm��)    
// 
// 
// 
// 
//***************************************************************************************
//GLM��GLSLʹ��mat4���ǰ������д洢�ģ���Ҫtransposeת��һ�¡�
//
//		ʾ����
//		//glm::mat4 example = glm::transpose(glm::mat4(
//		//	1.0f, 0.0f, 0.0f, 0.0f,
//		//	0.0f, 1.0f, 0.0f, 0.0f,
//		//	0.0f, 0.0f, 1.0f, 0.0f,
//		//	0.0f, 0.0f, 0.0f, 1.0f
//		//));
// 
// 
// 
// 
//***************************************************************************************



// ******************************** ��Ⱦϵͳ���� ***************************************


class RenderSystem
{
public:
	static RenderSystem* instance;
	static RenderSystem* Instance();

	// ------ ��Ļ/���� ------

	////��Ļ�ߴ磨��Ҫ֡���壩
	int frame_buffer_width;
	int frame_buffer_height;
	
	//------- ȫ��Shader -------
	MaterialShader* shaderSkyBox;
	MaterialShader* shaderShadow;

	// -------- Bias���� ------
	glm::mat4 b;

	// ------- �������--------
	Camera * mainCamera;



	virtual void Initialize() = 0;
	virtual void Resize(int newWidth, int newHeight) = 0;
	virtual void Render(double time) = 0;

	void UpdateAllRenderers();
};






class OpenglRenderSystem : public RenderSystem
{
public:
	float ScreenAspect();


	void UpdateCameras();
	void RenderCameras();


	void Initialize();
	void Resize(int newWidth, int newHeight);
	void Render(double time);
};




// *************************************** �޳��Ϳɼ������ ***************************************

///��Ⱦ����  
class RenderScene
{
private:
	std::vector<Renderer*> renderers;
public:
	Light* mainLight;

public:
	static RenderScene* Instance();

public:
	std::vector<Renderer*>* GetInSceneRenders();

	void AddRenderer(Renderer* renderer);

	void RemoveFromScene(int nodeIdx);

	void UpdateSceneNode(int nodeIdx);
};

//���ӽڵ�  
class VisibleNode
{
public:
	Renderer* renderer;
	VisibleNode(Renderer* renderer);
};

//�޳�ģʽ  
enum class CullMode
{
	Default,
	Shadow,
};
//�޳����
class CullResult
{
public:
	std::vector<VisibleNode> nodes;
};

//��Ⱦ����
// ��ÿ��submeshһ����  
class RenderObject
{
public:
	VisibleNode* visibleNode;

	Material*	material;		
	int16		queueIndex;		//����index
	uint16		subsetIndex;	//������/���ʵ�index      

	float		distance;		//���ڼ�������
};

//��ȾPass  
// ��ÿ��submesh��ÿ��shaderpassһ����    
class RenderPass
{
public:
	int roIndex;
	uint16 passNumber;
	//bool firstPass;
	//bool multiPass;
};

//��Ӱ��ͼ    
class ShadowMap
{
public:
	Light* light;
	RenderTexture* texture;
	rew::AABB6f receiverBounds;

public:
	ShadowMap();
	~ShadowMap();
};




//��Ⱦ����  
//��Unity�У���ӦRenderLoop��  
//��Unity�У���DoForwardLoop�д��� �ֲ�����RenderLoop queue;�뿪DoForwardQueue�Զ�������  
class RenderQueue
{
public:
	std::vector<RenderObject> objects;
	std::vector<RenderPass> plainRenderPasses;
	ShadowMap mainShadowmap;
};
