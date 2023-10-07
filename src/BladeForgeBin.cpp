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
	// ������������� GLFW.
	glfwInit();
	
	/*
	* ��� ��� �������� API - vk, �� ���������� ���������
	* ���������� ����������� � OpenGL.
	*/
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// ������� ���� �������� �� �����.
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
