#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


class Application
{
private:
	static int targetFrameRate;
	static bool isEditor;
	static Application* instance;
public:
	static bool isPaused;
public:
	static Application* Intance();


	virtual void Run() = 0;
	virtual bool IsRunning() = 0;


	virtual void* GetGameWindowHandle() = 0;

	virtual void Resize(int width, int height) = 0;


	virtual void Update() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Quit() = 0;
};



class GlfwApplication : public Application
{
private:
	GLFWwindow* window;
public:
	void Run();
	bool IsRunning();
	
	void* GetGameWindowHandle();

	void Resize(int width, int height);

	void Update();
	void FixedUpdate();
	
	
	void Quit();

};

