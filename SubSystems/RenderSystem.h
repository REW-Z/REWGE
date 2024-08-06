#pragma once

//***************************************************************************************
//本引擎使用右手坐标系(Y-UP) (同glm库)    
// 
// 
// 
// 
//***************************************************************************************
//GLM和GLSL使用mat4都是按列排列存储的，需要transpose转置一下。
//
//		示例：
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



// ******************************** 渲染系统基类 ***************************************


class RenderSystem
{
public:
	static RenderSystem* instance;
	static RenderSystem* Instance();

	// ------ 屏幕/缓冲 ------

	////屏幕尺寸（主要帧缓冲）
	int frame_buffer_width;
	int frame_buffer_height;
	
	//------- 全局Shader -------
	MaterialShader* shaderSkyBox;
	MaterialShader* shaderShadow;

	// -------- Bias矩阵 ------
	glm::mat4 b;

	// ------- 主摄像机--------
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




// *************************************** 剔除和可见性相关 ***************************************

///渲染场景  
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

//可视节点  
class VisibleNode
{
public:
	Renderer* renderer;
	VisibleNode(Renderer* renderer);
};

//剔除模式  
enum class CullMode
{
	Default,
	Shadow,
};
//剔除结果
class CullResult
{
public:
	std::vector<VisibleNode> nodes;
};

//渲染对象
// （每个submesh一个）  
class RenderObject
{
public:
	VisibleNode* visibleNode;

	Material*	material;		
	int16		queueIndex;		//队列index
	uint16		subsetIndex;	//子网格/材质的index      

	float		distance;		//用于几何排序
};

//渲染Pass  
// （每个submesh下每个shaderpass一个）    
class RenderPass
{
public:
	int roIndex;
	uint16 passNumber;
	//bool firstPass;
	//bool multiPass;
};

//阴影贴图    
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




//渲染队列  
//（Unity中：对应RenderLoop）  
//（Unity中：在DoForwardLoop中创建 局部变量RenderLoop queue;离开DoForwardQueue自动析构）  
class RenderQueue
{
public:
	std::vector<RenderObject> objects;
	std::vector<RenderPass> plainRenderPasses;
	ShadowMap mainShadowmap;
};
