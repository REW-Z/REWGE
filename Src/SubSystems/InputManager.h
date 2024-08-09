#pragma once

enum  KeyEnum
{
	MOUSEL,
	MOUSER,
	MOUSEMID,

	NUM1,
	NUM2,
	NUM3,
	NUM4,
	NUM5,
	NUM6,
	NUM7,
	NUM8,
	NUM9,
	NUM0,
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,
	Z,
	X,
	C,
	V,
	B,
	N,
	M,
	SPACE,
	TAB,
	LSHIFT,
	LCTRL,
	LALT,
	BACKSPACE,
	ENTER,
	RSHIFT,
	RCTRL,
	RALT,
	UP,
	DOWN,
	LEFT,
	RIGHT,

	UNMAPPED,
};




class InputManager
{
	//ÐéÄâ°´¼ü£¿
public:
	static InputManager* Instance();

	//¼üÅÌ
	int keyStates[256];
	int keyDownThisFrame[256];
	int keyUpThisFrame[256];

	//Êó±ê
	rew::Vector2 mousePos;
	rew::Vector2 mouseMov;
	
	void Update();
};

class Input
{
public:
	static bool Key(KeyEnum key);
	static bool KeyDown(KeyEnum key);
	static bool KeyUp(KeyEnum key);

	static rew::Vector2 MousePos();
	static rew::Vector2 MouseMov();
};