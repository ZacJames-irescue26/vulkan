#pragma once
#include "Vulkan/engine.h"

class App
{
public:
	App(int width, int height, bool debug);
	~App();
	void run();

private:
	Engine* graphicsEngine;
	GLFWwindow* window;
	Scene* scene;

	double lastTime, currentTime;
	int numFrames;
	float frameTime;
	bool Running = true;
	void build_glfw_window(int width, int height, bool debugMode);
	void calculateFrameRate();
};