/*
* Auther: sNj
* Time Creation: 07.10.2023
*
*
* Description:
* Main part of application uses Vulkan API for rendering graphics.
* Class contains fundamental tools to interact with vk.
*
*/


#ifndef VULKAN_APP_H
#define VULKAN_APP_H

// Vulkan binaries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// IO-console output
#include <iostream>

// Standart exception
#include <stdexcept>

// Main-function return codes
#include <cstdlib>



/*
* –азмеры окна, пока что будут константой.
*/
const int WWIDTH = 800;
const int WHEIGHT = 600;



class BladeForge
{
public:

	/*
		Starts application.
	*/
	void run();


private:
	
	/*
	* Handles window.
	*/
	GLFWwindow* window;



	/*
	* Initialize Vulkan parts.
	*/
	void initVulkan();


	/*
	* Initialize GLFW-window parts.
	*/
	void initWindow();
	
	/*
	* Render loop.
	*/
	void mainLoop();
	
	/*
	* Terminates applicaion: stops all process and cleans memory.
	*/
	void cleanup();
};









#endif

