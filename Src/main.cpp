
//#define TEST_FUNC


#include "REWGE.h"

#include <ctime>
#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

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


#include "Memory/Allocator.h"
//#include "Memory//StackAllocator.hpp"


#include "Rendering/FBO.h"


#include "Utils.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/World.h"  

#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"
#include "SubSystems/Debug.h"
#include "SubSystems/Profiler.h"
#include "SubSystems/InputManager.h"
#include "SubSystems/WorldManager.h"
#include "SubSystems/TimeManager.h"

#include "SubSystems/RenderSystem.h"
#include "Application.h"


using namespace std;
using namespace glm;
using namespace rew;












//*************************************************************************************************
//*************************************************************************************************

#pragma region 场景初始化

GameObject* bomber = nullptr;
void sceneInit()
{
	//环境光
	World::GetCurrentWorld().worldSettings.envionmentAmbient = vec4(0.2f, 0.2f, 0.2f, 1.0f);

	//添加光源
	GameObject& goLight = GameObjectUtility::CreateGameObject("MainLight");
	goLight.transform->localRotation(glm::quatLookAt(normalize(glm::vec3(1.0f, -4.0f, 1.0f)), glm::vec3(0.0f, 1.0f, 0.0f)));
	Light* lightCom = dynamic_cast<Light*>( goLight.AddComponent(TypeEnum::TYPE_Light));
	lightCom->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	
	//添加摄像机
	GameObject& goCam = GameObjectUtility::CreateGameObject("MainCam");
	auto camCom = dynamic_cast<Camera*> (goCam.AddComponent(TypeEnum::TYPE_Camera));
	camCom->gameObject->transform->localPosition(0.0f, 0.0f, 0.0f);
	camCom->gameObject->transform->localEularAngles(0,0,0);

	RenderSystem::Instance()->mainCamera = camCom;
	RenderScene::Instance()->mainLight = lightCom;

	GameObject* obj0 = ResourceImporter::ImportObjModel("Resources\\plane.obj");
	GameObject* obj1 = ResourceImporter::ImportObjModel("Resources\\sundial.obj");
	GameObject* obj2 = ResourceImporter::ImportObjModel("Resources\\cubes.obj");
	GameObject* obj3 = ResourceImporter::ImportObjModel("Resources\\Bomber.obj");

	WorldManager::Instance().currentWorld.AddGameObject(*obj0);
	WorldManager::Instance().currentWorld.AddGameObject(*obj1);
	WorldManager::Instance().currentWorld.AddGameObject(*obj2);
	WorldManager::Instance().currentWorld.AddGameObject(*obj3);



	obj3->transform->localPosition(20, 0, 0);
	obj3->transform->localRotation(goLight.transform->localRotation());

	bomber = obj3;
}

#pragma endregion

#pragma region 游戏循环相关
//FIXED UPDATE
void fixed_update()
{

}


//UPDATE
void update()
{
	//脚本运行时更新  
	//...

	//光源旋转
	Light* light = RenderScene::Instance()->mainLight; assert(light);
	light->gameObject->transform->RotateAxisAngle(glm::vec3(0.0f,1.0f,0.0f), 0.025f, Transform::Space::World);

	//飞机旋转
	if(bomber) bomber->transform->RotateAxisAngle(glm::vec3(0.0f,1.0f,0.0f), 0.025f, Transform::Space::Local);



	//移动控制
	Camera* cam = RenderSystem::Instance()->mainCamera;
	auto camTransform = RenderSystem::Instance()->mainCamera->gameObject->transform;

	if (Input::Key(KeyEnum::W))
	{
		camTransform ->localPosition(camTransform->localPosition() + glm::normalize(camTransform->forward()) * 0.1f);
	}
	if (Input::Key(KeyEnum::S))
	{
		camTransform->localPosition(camTransform->localPosition() - glm::normalize(camTransform->forward()) * 0.1f);
	}
	if (Input::Key(KeyEnum::A))
	{
		camTransform->localPosition(camTransform->localPosition() - glm::normalize(camTransform->right()) * 0.1f);
	}
	if (Input::Key(KeyEnum::D))
	{
		camTransform->localPosition(camTransform->localPosition() + glm::normalize(camTransform->right()) * 0.1f);
	}
	if (Input::Key(KeyEnum::F))
	{
		camTransform->localPosition(camTransform->localPosition() + glm::normalize(camTransform->up()) * 0.1f);
	}
	if (Input::Key(KeyEnum::C))
	{
		camTransform->localPosition(camTransform->localPosition() - glm::normalize(camTransform->up()) * 0.1f);
	}

	rew::Vector2 mousePos = Input::MousePos();
	rew::Vector2 mouseMov = Input::MouseMov();

	if (Input::Key(KeyEnum::MOUSEL))
	{
		camTransform->RotateAxisAngle(vec3(0.0, 1.0, 0.0), -mouseMov.x * 0.2f, Transform::World);
		camTransform->RotateAxisAngle(vec3(1.0, 0.0, 0.0), -mouseMov.y * 0.2f, Transform::Local);
	}
}

#pragma endregion



struct TestStruct
{
public:
	float a;
	char b;
	float c;
};


// 示例和测试  
void Test()
{
	//PROFILE("Test");


	// --- 测试按类型遍历搜索文件 ---

	std::vector<string> files;
	FileSystem::FindAllFilesWithRegex("Resources", regex(".obj"), files);

	for (auto f: files)
	{
		printf(f.c_str());
	}

	//// --- 测试使用Debug.Log和输出功能 ---
	//  
	//string str("aaaaaaaaa");
	//str.append("123123213213213");
	//Debug::Log(str.c_str());
	//Debug::WriteLogFile();


	//// --- 测试载入和卸载资源---
	// 
	//Object * asset = ResourceManager::Load(TypeEnum::TYPE_Texture2D, "Resources\\default.jpg");
	//asset->Free();


	//// --- 测试载入重复资源---
	// 
	//Asset * asset = ResourceManager::Load<Texture2D>("Resources\\default.jpg");
	//Asset * asset2 = ResourceManager::Load<Texture2D>("Resources\\default.jpg");
	//Debug::Log(to_string((long long)asset).c_str());
	//Debug::Log(to_string((long long)asset2).c_str());


	//// ---- 编译C#脚本 -----
	// 
	//system("Scripts\\compile.bat");


	////// ----- 测试反射 -----
	// 
	//Object * com =  Activator::CreateInstace(&Component::type);
	//Type * type = com->GetType();
	//cout << "类型名1：" << type->name << endl;

	//Object* beh = Activator::CreateInstace("Behaviour");
	//Type* type2 = beh->GetType();
	//cout << "类型名2：" << type2->name << endl;

	//cout << "beh->GetTypeEnum" << beh->GetTypeEnum() << endl;
	//cout << "beh->Is(TYPE_Component)" << beh->Is(TYPE_Component) << endl;
	//cout << "beh->Is(TYPE_GameObject)" << beh->Is(TYPE_GameObject) << endl;
	//cout << "beh->Is(TYPE_Object)" << beh->Is(TYPE_Object) << endl;

	//cout << "type2->name" << type2->name << endl;
	//cout << "type2->typeEnum" << to_string( type2->typeEnum ) << endl;
	//cout << "type2->baseType->name" << type2->baseType->name << endl;
	//cout << "type2->Is(TYPE_Component)" << type2->Is(TYPE_Component) << endl;
	//cout << "type2->Is(TYPE_GameObject)" << type2->Is(TYPE_GameObject) << endl;
	//cout << "type2->Is(TYPE_Object)" << type2->Is(TYPE_Object) << endl;

	//cout << "beh is (Component)" << beh is (Component) << endl;
	//cout << "beh is (GameObject)" << beh is (GameObject) << endl;

	//cout << "typeof(Behaviour)" << (typeof(Behaviour))->name << endl;
	

	//// ----- AS -----
	// 
	//Object* beh = Object::CreateInstance(TYPE_Transform);
	//cout << "\"as\" test: " << (beh as(Transform))->localPosition().x << endl;



	//// --------- OPTR -----------
	//
	//OPtr<Texture2D> tex = StringId("Resources\\vstol.png");

	//Texture2D* tex2 = (tex);
	//int w = (*tex).width;
	//int w2 = tex->width;
	//cout << w << endl;
	//cout << w2 << endl;

	//// ------------- VEC Parse --------------------
	// 
	//glm::vec3 vec = glm::vec3(3.0f, 4.0f, 5.0f);
	//rew::Vector3 vecConvert = RewVec3(vec);
	//rew::Vector3* asGlmVec = AsRewVec3(vec);

	//cout << "vecConvert" << vecConvert.x << "," << vecConvert.y << "," << vecConvert.z << endl;
	//cout << "asGlmVec" << asGlmVec->x << "," << asGlmVec->y << "," << asGlmVec->z << endl;


	//std::cout << Path::GetFullNameWithOutExtension("C:\\FDSFDSF\\dsfsdf.txt") << std::endl;


	system("pause");
}



int main(void)
{
	//初始化脚本运行时
	//...

	//运行GLFW应用程序 （确定窗口尺寸）
	Application::Intance()->Run();


	//测试
#ifdef TEST_FUNC
	Test();
	return 0;
#endif

	//渲染系统初始化
	RenderSystem::Instance()->Initialize();


	//游戏场景初始化
	sceneInit();

	//测试脚本运行时
	//...


#ifdef EDITOR
	//运行游戏编辑器
	//...  
#endif // EDITOR


	//-----------------------------------------------------------------------------

	//游戏循环GameLoop
	while (Application::Intance()->IsRunning())
	{
		//Pause  
		if (Input::Key(KeyEnum::P))
		{
			Application::isPaused = true;
		}
		if (Application::isPaused) continue;


		//TIME
		TimeManager::UpdateTime(std::clock());


		//FIXED UPDATE:
		while (TimeManager::simulationTime < TimeManager::gameTime)
		{
			//ENGINE FIXED UPDATE
			fixed_update();

			//APP FIXED UPDATE
			Application::Intance()->FixedUpdate();

			TimeManager::FixedUpateSimTime();
		}


		//UPDATE:
		update();

		//SCENE UPDATE
		RenderSystem::Instance()->UpdateAllRenderers();


		//APP Update:
		Application::Intance()->Update();
	}


	//-----------------------------------------------------------------------------

	//应用程序接口退出
	Application::Intance()->Quit();

	//EXIT
	exit(EXIT_SUCCESS);

	//脚本运行时-释放  
	//...


	//输出性能剖析器日志
#ifdef LOG_PROFILER
	Profiler::WriteProfileLog();
#endif
	

	return 0;
}