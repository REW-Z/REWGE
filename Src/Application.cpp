

#include "REWGE.h"

#ifdef WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif



#include <ctime>
#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

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



#include "Application.h"

#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"
#include "SubSystems/Debug.h"
#include "SubSystems/Profiler.h"
#include "SubSystems/InputManager.h"
#include "SubSystems/RenderSystem.h"

#include "Application.h"



bool Application::isPaused = false;

Application* Application::instance = NULL;

Application* Application::Intance()
{
	if (Application::instance == NULL)
		instance = new GlfwApplication();
	return Application::instance;
}
//
//void Application::Run()
//{
//}
//
//bool Application::IsRunning()
//{
//	return false;
//}
//
//void Application::Update()
//{
//}
//
//void Application::Quit()
//{
//}









KeyEnum GLFWKeyMap[512];

KeyEnum GLFWMouseKeyMap[32];

//映射表初始化
void keyMapInit()
{
	for (int i = 0; i < 32; i++) { GLFWMouseKeyMap[i] = KeyEnum::UNMAPPED; }

	GLFWMouseKeyMap[GLFW_MOUSE_BUTTON_LEFT] = KeyEnum::MOUSEL;
	GLFWMouseKeyMap[GLFW_MOUSE_BUTTON_RIGHT] = KeyEnum::MOUSER;
	GLFWMouseKeyMap[GLFW_MOUSE_BUTTON_MIDDLE] = KeyEnum::MOUSEMID;


	for (int i = 0; i < 512; i++) { GLFWKeyMap[i] = KeyEnum::UNMAPPED; }

	GLFWKeyMap[GLFW_MOUSE_BUTTON_LEFT] = KeyEnum::MOUSEL;
	GLFWKeyMap[GLFW_MOUSE_BUTTON_RIGHT] = KeyEnum::MOUSER;
	GLFWKeyMap[GLFW_MOUSE_BUTTON_MIDDLE] = KeyEnum::MOUSEMID;

	GLFWKeyMap[GLFW_KEY_1] = KeyEnum::NUM1;
	GLFWKeyMap[GLFW_KEY_2] = KeyEnum::NUM2;
	GLFWKeyMap[GLFW_KEY_3] = KeyEnum::NUM3;
	GLFWKeyMap[GLFW_KEY_4] = KeyEnum::NUM4;
	GLFWKeyMap[GLFW_KEY_5] = KeyEnum::NUM5;
	GLFWKeyMap[GLFW_KEY_6] = KeyEnum::NUM6;
	GLFWKeyMap[GLFW_KEY_7] = KeyEnum::NUM7;
	GLFWKeyMap[GLFW_KEY_8] = KeyEnum::NUM8;
	GLFWKeyMap[GLFW_KEY_9] = KeyEnum::NUM9;
	GLFWKeyMap[GLFW_KEY_0] = KeyEnum::NUM0;
	GLFWKeyMap[GLFW_KEY_Q] = KeyEnum::Q;
	GLFWKeyMap[GLFW_KEY_W] = KeyEnum::W;
	GLFWKeyMap[GLFW_KEY_E] = KeyEnum::E;
	GLFWKeyMap[GLFW_KEY_R] = KeyEnum::R;
	GLFWKeyMap[GLFW_KEY_T] = KeyEnum::T;
	GLFWKeyMap[GLFW_KEY_Y] = KeyEnum::Y;
	GLFWKeyMap[GLFW_KEY_U] = KeyEnum::U;
	GLFWKeyMap[GLFW_KEY_I] = KeyEnum::I;
	GLFWKeyMap[GLFW_KEY_O] = KeyEnum::O;
	GLFWKeyMap[GLFW_KEY_P] = KeyEnum::P;
	GLFWKeyMap[GLFW_KEY_A] = KeyEnum::A;
	GLFWKeyMap[GLFW_KEY_S] = KeyEnum::S;
	GLFWKeyMap[GLFW_KEY_D] = KeyEnum::D;
	GLFWKeyMap[GLFW_KEY_F] = KeyEnum::F;
	GLFWKeyMap[GLFW_KEY_G] = KeyEnum::G;
	GLFWKeyMap[GLFW_KEY_H] = KeyEnum::H;
	GLFWKeyMap[GLFW_KEY_J] = KeyEnum::J;
	GLFWKeyMap[GLFW_KEY_K] = KeyEnum::K;
	GLFWKeyMap[GLFW_KEY_L] = KeyEnum::L;
	GLFWKeyMap[GLFW_KEY_Z] = KeyEnum::Z;
	GLFWKeyMap[GLFW_KEY_X] = KeyEnum::X;
	GLFWKeyMap[GLFW_KEY_C] = KeyEnum::C;
	GLFWKeyMap[GLFW_KEY_V] = KeyEnum::V;
	GLFWKeyMap[GLFW_KEY_B] = KeyEnum::B;
	GLFWKeyMap[GLFW_KEY_N] = KeyEnum::N;
	GLFWKeyMap[GLFW_KEY_M] = KeyEnum::M;
	GLFWKeyMap[GLFW_KEY_SPACE] = KeyEnum::SPACE;
	GLFWKeyMap[GLFW_KEY_TAB] = KeyEnum::TAB;
	GLFWKeyMap[GLFW_KEY_LEFT_SHIFT] = KeyEnum::LSHIFT;
	GLFWKeyMap[GLFW_KEY_LEFT_CONTROL] = KeyEnum::LCTRL;
	GLFWKeyMap[GLFW_KEY_LEFT_ALT] = KeyEnum::LALT;
	GLFWKeyMap[GLFW_KEY_BACKSPACE] = KeyEnum::BACKSPACE;
	GLFWKeyMap[GLFW_KEY_ENTER] = KeyEnum::ENTER;
	GLFWKeyMap[GLFW_KEY_RIGHT_SHIFT] = KeyEnum::RSHIFT;
	GLFWKeyMap[GLFW_KEY_RIGHT_CONTROL] = KeyEnum::RCTRL;
	GLFWKeyMap[GLFW_KEY_RIGHT_ALT] = KeyEnum::RALT;
	GLFWKeyMap[GLFW_KEY_UP] = KeyEnum::UP;
	GLFWKeyMap[GLFW_KEY_DOWN] = KeyEnum::DOWN;
	GLFWKeyMap[GLFW_KEY_LEFT] = KeyEnum::LEFT;
	GLFWKeyMap[GLFW_KEY_RIGHT] = KeyEnum::RIGHT;
}
//按键回调
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		InputManager::Instance()->keyStates[GLFWKeyMap[key]] = 1;
		InputManager::Instance()->keyDownThisFrame[GLFWKeyMap[key]] = 1;
	}
	if (action == GLFW_RELEASE)
	{
		InputManager::Instance()->keyStates[GLFWKeyMap[key]] = 0;
		InputManager::Instance()->keyUpThisFrame[GLFWKeyMap[key]] = 1;
	}
}
//鼠标按键回调  
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		InputManager::Instance()->keyStates[GLFWMouseKeyMap[button]] = 1;
		InputManager::Instance()->keyDownThisFrame[GLFWMouseKeyMap[button]] = 1;
	}
	if (action == GLFW_RELEASE)
	{
		InputManager::Instance()->keyStates[GLFWMouseKeyMap[button]] = 0;
		InputManager::Instance()->keyUpThisFrame[GLFWMouseKeyMap[button]] = 1;
	}
}
//鼠标回调
void curse_poscallback(GLFWwindow* window, double x, double y)
{
	InputManager::Instance()->mouseMov.x += (float)x - InputManager::Instance()->mousePos.x;
	InputManager::Instance()->mouseMov.y += (float)y - InputManager::Instance()->mousePos.y;

	InputManager::Instance()->mousePos.x = (float)x;
	InputManager::Instance()->mousePos.y = (float)y;
}

//size回调(window和framebuff的size事件传回的长宽是一样的)  
void window_size_callback(GLFWwindow* window, int new_width, int new_height)
{
}
void frame_buffer_size_callback(GLFWwindow* window, int new_width, int new_height)
{
	Application::Intance()->Resize(new_width, new_height);
}


void GlfwApplication::Run()
{
	//初始化GLFW
	if (!glfwInit()) { exit(EXIT_FAILURE); }


	//实例化窗口(4.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//窗口选项：主版本号4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//窗口选项：次版本号3
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GameEngine", NULL, NULL);
	
	//设置为应用程序当前窗口
	this->window = window;

	//将将GLFW窗口与当前OpenGL上下文联系起来
	glfwMakeContextCurrent(window);

	//初始化GLEW
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	
	
	//设置交换缓冲区前等待的帧数为1（开启垂直同步）  ==>使用glfwSwapBuffers()交换缓冲区
	glfwSwapInterval(0);


	//按键映射
	keyMapInit(); 
	//设置输入回调
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, curse_poscallback);
	//设置Resize回调
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

	
	//缓冲区尺寸
	int frameBufferW, frameBufferH;
	glfwGetFramebufferSize(window, &frameBufferW, &frameBufferH);
	Resize(frameBufferW, frameBufferH);
}

bool GlfwApplication::IsRunning()
{
	return !glfwWindowShouldClose(this->window);
}

void* GlfwApplication::GetGameWindowHandle()
{
#ifdef WIN32
	HWND handle = glfwGetWin32Window(window);
	return (void*)handle;
#endif
	return nullptr;
}


void GlfwApplication::Resize(int width, int height)
{
	//Debug::Log("GLFW Window Resize to" + to_string(width) + to_string(height));

	RenderSystem::Instance()->Resize(width, height);
}



void GlfwApplication::Update()
{
	//输入系统更新
	InputManager::Instance()->Update();

	//OPENGL渲染
	RenderSystem::Instance()->Render(glfwGetTime());

	//交换缓冲区(会等待Interval)
	glfwSwapBuffers(window);


	//处理窗口事件
	glfwPollEvents();
}

void GlfwApplication::FixedUpdate()
{

}

void GlfwApplication::Quit()
{
	//退出
	glfwDestroyWindow(this->window);
	glfwTerminate();
}
