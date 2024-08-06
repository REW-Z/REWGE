
#include <iostream>
#include "REWCPPL.h"
#include "InputManager.h"


InputManager* InputManager::Instance()
{
	static InputManager* inst = new InputManager();
	return inst;
}


void InputManager::Update()
{
	mouseMov.x = 0.0f;
	mouseMov.y = 0.0f;
	for (int i = 0; i < 256; i++)
	{
		keyUpThisFrame[i] = 0;
		keyDownThisFrame[i] = 0;
	}
}


bool Input::Key(KeyEnum key)
{
	return InputManager::Instance()->keyStates[key];
}


bool Input::KeyDown(KeyEnum key)
{
	return InputManager::Instance()->keyDownThisFrame[key];
}


bool Input::KeyUp(KeyEnum key)
{
	return InputManager::Instance()->keyUpThisFrame[key];
}

rew::Vector2 Input::MousePos()
{
	return InputManager::Instance()->mousePos;
}
rew::Vector2 Input::MouseMov()
{
	return InputManager::Instance()->mouseMov;
}