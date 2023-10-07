#include "BladeForgeBin.h"

void BladeForge::run()
{
	initVulkan();
	mainLoop();
	cleanup();
}

void BladeForge::initVulkan()
{
}

void BladeForge::initWindow()
{
	// Инициализация GLFW.
	glfwInit();
	
	/*
	* Так как основное API - vk, то необходимо выключить
	* встроенное подключение к OpenGL.
	*/
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// Размеры окна меняться не будут.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WWIDTH, WHEIGHT, "BladeForge", nullptr, nullptr);

}

void BladeForge::mainLoop()
{
	// Main Render Loop.
	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
	}

}

void BladeForge::cleanup()
{
}
