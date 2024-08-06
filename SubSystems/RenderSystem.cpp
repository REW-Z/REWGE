
#include "REWGE.h"

#include <ctime>
#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>


#include <iostream>
#include <string>
#include <regex>
#include <sstream>
#include <vector>
#include <list>
#include <unordered_map>
#include <cmath>
#include <stack>
#include <functional>

#include "REWCPPL.h"


#include "Utils.h"

#include "Memory/Allocator.h"

#include "Rendering/FBO.h"

#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Texture.h"
#include "Objects/RenderTexture.h"
#include "Objects/Shader.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/Renderer.h"
#include "Objects/World.h"

#include "Application.h"

#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"
#include "SubSystems/WorldManager.h"
#include "SubSystems/Debug.h"
#include "SubSystems/Profiler.h"




#include "RenderSystem.h"

using namespace glm;





//------------------------------ UTILS -----------------------------------

#pragma region 纹理读取

GLuint loadTexture(const char* texImagePath)
{
	GLuint textureID;
	textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) cout << "couldn't find texture file:  " << texImagePath << endl;
	// MipMap
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//各向异性过滤
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
	}
	//ClampOrRepeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	cout << "已读取贴图:  " << texImagePath << endl;
	return textureID;
}

GLuint loadCubeMap(const char* mapDir)
{
	GLuint textureRef;
	string xp = mapDir; xp = xp + "/xp.jpg";
	string xn = mapDir; xn = xn + "/xn.jpg";
	string yp = mapDir; yp = yp + "/yp.jpg";
	string yn = mapDir; yn = yn + "/yn.jpg";
	string zp = mapDir; zp = zp + "/zp.jpg";
	string zn = mapDir; zn = zn + "/zn.jpg";
	textureRef = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(), zp.c_str(), zn.c_str(),
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (textureRef == 0) cout << "didnt find cube map image file:  " << mapDir << endl;
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);
	// reduce seams
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	cout << "已读取立方贴图:  " << mapDir << endl;
	return textureRef;
}


#pragma endregion

// -------------------------------------------- Forward -------------------------------------------------

void RenderCamera(Camera* cam);



// --------------------------------------------- RenderSystem -----------------------------------------------

RenderSystem* RenderSystem::instance = NULL;

RenderSystem* RenderSystem::Instance()
{
	if (RenderSystem::instance == NULL)
		RenderSystem::instance = new OpenglRenderSystem();
	return RenderSystem::instance;
}

void RenderSystem::UpdateAllRenderers()
{
	auto list = Renderer::GetRendererList();
	for (auto it = list->begin(); it != list->end(); ++it)
	{
		if (!(*it)) continue;

		Renderer* renderer = (*it);
		if (renderer->ShouldInScene())
		{
			if (!renderer->IsInScene())
			{
				//add to scene
				RenderScene::Instance()->AddRenderer(renderer);
				assert(renderer->GetNodeIdx() > -1);
			}
			//update node(idx)
			assert(renderer->GetNodeIdx() > -1);
			RenderScene::Instance()->UpdateSceneNode(renderer->GetNodeIdx());
		}
		else
		{
			//remove from scene
			RenderScene::Instance()->RemoveFromScene(renderer->GetNodeIdx());
		}
	}
}



// --------------------------------------------- OpenglRenderSystem -----------------------------------------------


float OpenglRenderSystem::ScreenAspect()
{
	return (float)frame_buffer_width / (float)frame_buffer_height;
}

//初始化
void  OpenglRenderSystem::Initialize()
{
	//默认纹理
	Texture2D::emptyTexture = ResourceManager::Load(TypeEnum::TYPE_Texture2D, "Resources\\default.jpg") as(Texture2D);;
	//texSkybox = loadCubeMap("Resources\\cubeMap");
	World::GetCurrentWorld().worldSettings.texSkybox = loadCubeMap("Resources\\cubeMap");

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);//开启无缝

	//texEmpty = loadTexture("Resources\\default.jpg");
	//tex1 = texEmpty; tex2 = texEmpty;

	//编译着色器（渲染程序）
	this->shaderSkyBox = ResourceManager::Load(TypeEnum::TYPE_MaterialShader, "Shaders\\Cubemap") as(MaterialShader);
	this->shaderShadow = ResourceManager::Load(TypeEnum::TYPE_MaterialShader, "Shaders\\Shadow") as(MaterialShader);
	//this->shaderBlinnPhong = ResourceManager::Load(TypeEnum::TYPE_Shader, "Shaders\\BlinnPhong") as(Shader);

	//通用变量设置
	b = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);

	//Resize事件重置摄像机
	Resize(frame_buffer_width, frame_buffer_height);
}

void OpenglRenderSystem::Resize(int newWidth, int newHeight)
{
	//重设帧缓冲尺寸
	frame_buffer_width = newWidth;
	frame_buffer_height = newHeight;

	Debug::Log("frame_w:" + to_string(frame_buffer_width));

	//更新摄像机参数
	UpdateCameras();
}

void OpenglRenderSystem::UpdateCameras()
{
	if (mainCamera == nullptr) return;

	//主摄像机计算Aspect
	mainCamera->aspectRatio = ScreenAspect();

	//主摄像机渲染默认缓冲区
	if (mainCamera->renderTarget != FBO::Default())
	{
		mainCamera->renderTarget = FBO::Default();
	}
}
void OpenglRenderSystem::RenderCameras()
{
	//TODO:RenderCameras
	Camera* cam = mainCamera;
	{ 
		RenderCamera(cam);
	}
}
void OpenglRenderSystem::Render(double time)
{
	//更新所有摄像机参数
	UpdateCameras();

	//渲染所有摄像机
	RenderCameras();
}



// ------------------ 剔除和可见性相关  ------------------------

RenderScene* RenderScene::Instance()
{
	static RenderScene scene = RenderScene();
	return &scene;
}

std::vector<Renderer*>* RenderScene::GetInSceneRenders()
{
	return &renderers;
}

void RenderScene::AddRenderer(Renderer* renderer)
{
	this->renderers.push_back(renderer);

	//set is in scene
	int idx = this->renderers.size() - 1;
	assert(idx > -1);
	renderer->SetIsInScene(true, idx);
}

void RenderScene::RemoveFromScene(int nodeIdx)
{
	assert((int)renderers.size() > nodeIdx);
	assert((int)renderers.size() > 0);

	//target renderer
	auto targetRenderer = this->renderers[nodeIdx];

	//half - swap
	int lastIdx = renderers.size() - 1;
	renderers[nodeIdx] = renderers[lastIdx];

	//Set In Scene
	targetRenderer->SetIsInScene(false, -1);

	//remove
	this->renderers.pop_back();
}

void RenderScene::UpdateSceneNode(int nodeIdx)
{
	assert((int)renderers.size() > nodeIdx);
	assert((int)renderers.size() > 0);

	//update aabb...
	renderers[nodeIdx]->UpdateAABB();
	//update layer
	renderers[nodeIdx]->SetLayer(renderers[nodeIdx]->gameObject->layer);
}


VisibleNode::VisibleNode(Renderer* renderer)
{
	this->renderer = renderer;
}


void Cull(CullResult* cullresults, Camera* cam, CullMode mode)
{
	//Clear
	cullresults->nodes.clear();

	//Cull  
	switch (mode)
	{
	case CullMode::Shadow:
	{
		auto renderers = RenderScene::Instance()->GetInSceneRenders();
		for (auto it : *renderers)
		{
			cullresults->nodes.push_back(VisibleNode(it));
		}
	}
	break;
	case CullMode::Default:
	default:
	{
		auto renderers = RenderScene::Instance()->GetInSceneRenders();
		for (auto it : *renderers)
		{
			Renderer* renderer = it;
			auto worldAABB = renderer->GetWorldAABB();
			if (cam->IsInFrustum(worldAABB))
			{
				cullresults->nodes.push_back(VisibleNode(renderer));
			}
		}
	}
	break;
	}
}




/// **************************** ShadowMap **************************************

ShadowMap::ShadowMap()
{
	int w = RenderSystem::Instance()->frame_buffer_width;
	int h = RenderSystem::Instance()->frame_buffer_height;
	RenderTexture* shadowMap = RenderTexture::GetTemp(w, h);
	this->texture = shadowMap;
}

ShadowMap::~ShadowMap()
{
	if (this->texture != nullptr)
	{
		RenderTexture::ReleaseTemp(this->texture);
	}
}


// ------------------------------------------------------- 渲染相关 ---------------------------------------------------------


// ---------- 天空盒相关 ---------------

GLuint skyVBO;
GLuint skyVAO;

bool is_bind_skybox_data = false;

float cubeVertexPositions[108] =
{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
	1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
	1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
};

void bindSkyboxData()
{
	if (is_bind_skybox_data) return;

	glGenVertexArrays(1, &skyVAO);
	glBindVertexArray(skyVAO);

	glGenBuffers(1, &skyVBO);  //第100个网格放置天空盒

	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);



	is_bind_skybox_data = true;
}


// ---------- 来自Renderer.cpp的外部通用变量 ---------------

extern glm::mat4 mMat, vMat, mvMat, pMat, invTrMat;
extern GLuint mLoc, vLoc, mvLoc, projLoc, nLoc, sLoc;
extern glm::mat4 lightVmatrix;
extern glm::mat4 lightPmatrix;

// ------------ 通用CullResults ----------------
CullResult* shadowCullResult = NEW(CullResult) CullResult();
CullResult* mainCullResult = NEW(CullResult) CullResult();





//几何排序器  
struct RenderObjectSorter
{
	bool operator()(const RenderPass& ra, const RenderPass& rb) const;
	const RenderQueue* queue;
};
bool RenderObjectSorter::operator()(const RenderPass& ra, const RenderPass& rb) const
{
	const RenderObject& dataa = (queue->objects)[ra.roIndex];
	const RenderObject& datab = (queue->objects)[rb.roIndex];

	return dataa.distance < datab.distance;
}



//渲染天空盒
void pass_skybox()
{
}


//阴影渲染
void pass_shadow(Camera* cam)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Cull  
	Cull(shadowCullResult, cam, CullMode::Shadow);

	//Render  
	for (auto it : shadowCullResult->nodes)
	{
		MeshRenderer* renderer = dynamic_cast<MeshRenderer*>(it.renderer);

		if (renderer != nullptr && renderer->mesh != nullptr)
		{
			renderer->DrawShadow();
		}
	}
}

//主渲染PASS（渲染不透明物体）  
void pass_main(Camera* cam, RenderQueue* queue)
{
	//设置
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	for (auto& it : queue->plainRenderPasses)
	{
		auto& ro = queue->objects[it.roIndex];
		Renderer* renderer = ro.visibleNode->renderer;

		renderer->Draw();
	}
}


//渲染相机
void RenderCamera(Camera* cam)
{
	//绑定当前相机的帧缓冲
	cam->renderTarget->Bind();


	//清除颜色缓存和深度缓冲
	if (cam->cleanColorBuffer)
	{
		glClearColor(cam->bgColor.r, cam->bgColor.g, cam->bgColor.b, cam->bgColor.a);//设置清除色
		glClear(GL_COLOR_BUFFER_BIT); //GL_COLOR_BUFFER_BIT包含了渲染后像素的颜色缓冲区。
	}
	if (cam->cleanDepthBuffer)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}


	//世界设置
	auto worldSettings = World::GetCurrentWorld().worldSettings;


	// ***************** 渲染队列创建 *****************      
	RenderQueue queue;

	{
		//摄像机剔除      
		Cull(mainCullResult, cam, CullMode::Default);

		//初始化RenderObject列表  
		for (int i = 0; i < mainCullResult->nodes.size(); ++i)
		{
			auto renderer = mainCullResult->nodes[i].renderer;
			float distToCam = glm::length(renderer->gameObject->transform->worldPosition() - cam->gameObject->transform->worldPosition());

			for (int j = 0; j < renderer->materials.size(); ++j)
			{
				auto mat = mainCullResult->nodes[i].renderer->materials[j];

				RenderObject& roNew = queue.objects.emplace_back();
				roNew.visibleNode = &(mainCullResult->nodes[i]);
				roNew.subsetIndex = j;
				roNew.material = mat;
				roNew.queueIndex = mat->queueidx;
				roNew.distance = distToCam;
			}
		}

		//初始化RenderPass列表  
		for (int i = 0; i < queue.objects.size(); ++i)
		{
			Material* mat = queue.objects[i].material;

			for (int j = 0; j < mat->shader->passes.size(); ++j)
			{
				RenderPass& pass = queue.plainRenderPasses.emplace_back();
				pass.passNumber = j;
				pass.roIndex = i;
			}
		}

		//几何排序  
		RenderObjectSorter sorter;
		sorter.queue = &queue;
		std::sort(queue.plainRenderPasses.begin(), queue.plainRenderPasses.end(), sorter);
	}


	//********** 天空盒 *********************************
	RenderSystem::Instance()->shaderSkyBox->passes[0]->Use();


	//生成天空盒网格并绑定
	if (!is_bind_skybox_data) bindSkyboxData();

	pMat = cam->GetMatrixP();
	vMat = cam->GetMatrixV();


	vLoc = glGetUniformLocation(RenderSystem::Instance()->shaderSkyBox->passes[0]->renderProgram, "v_matrix");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(RenderSystem::Instance()->shaderSkyBox->passes[0]->renderProgram, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, worldSettings.texSkybox);


	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(skyVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);//移到pass_skybox(或者sky_bg)
	glEnable(GL_DEPTH_TEST);
	//****************************************************


	//**********  PASS_SHADOW  ******************************

	//光源获取
	Light* mainLight = RenderScene::Instance()->mainLight;

	//光源空间的变换矩阵构建
	lightVmatrix = glm::lookAt(cam->gameObject->transform->worldPosition(), cam->gameObject->transform->worldPosition() + mainLight->dir(), glm::vec3(0.0, 1.0, 0.0));
	lightPmatrix = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);


	//使用自定义的阴影帧缓冲区，并把阴影纹理附着在上面
	glBindFramebuffer(GL_FRAMEBUFFER, queue.mainShadowmap.texture->fbo->fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, queue.mainShadowmap.texture->tex, 0);

	glDrawBuffer(GL_NONE);		//关闭绘制颜色
	glEnable(GL_DEPTH_TEST);		//开启深度测试
	glEnable(GL_POLYGON_OFFSET_FILL);	// 开启深度偏移
	glPolygonOffset(1.0f, 2.0f);		//  深度偏移

	pass_shadow(cam);

	glDisable(GL_POLYGON_OFFSET_FILL);	// 关闭深度偏移
	//***************************************************

	//**********  PASS_MAIN  ******************************

	//绑定当前相机的帧缓冲
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	cam->renderTarget->Bind();

	glActiveTexture(GL_TEXTURE0);			//纹理单元0-绑定阴影纹理
	glBindTexture(GL_TEXTURE_2D, queue.mainShadowmap.texture->tex);


	glDrawBuffer(GL_FRONT);//重新开启绘制颜色

	//pass
	pass_main(cam, &queue);
	//***************************************************
}
