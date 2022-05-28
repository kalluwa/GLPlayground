#include "LiverViewer.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <TextureLoader.h>

#include <test_unit.h>

#include <GameApp.h>
#include <fmt/core.h>
#include <chrono>


std::unique_ptr<GameApp> game_app = nullptr;

int g_button = -1;
bool g_button_down=false;
double mousx,mousy;
bool mouse_button_down =false;
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if(g_button_down)
		game_app->inputMouse(xpos, ypos, -1, -1);
};

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	game_app->inputMouseScroll(yoffset>0?120:-120);
};

void mousebutton_callback (GLFWwindow *window, int button, int action, int mods)
{
	g_button = button;
	g_button_down = GLFW_PRESS == action;


	glfwGetCursorPos(window, &mousx, &mousy);
	game_app->inputMouse(mousx, mousy, button, GLFW_PRESS == action);

}
int main()
{
	GLFWwindow* window = nullptr;
	if(!glfwInit())
		return -1;

	std::string title = "LiverViewer";

	int width = 800;
	int height = 600;
	window = glfwCreateWindow(width,height,title.c_str(),nullptr,nullptr);
	if(!window)
	{
		glfwTerminate();
		return -1;
	}

	//绑定当前window
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window,mousebutton_callback);
	//初始化实例
	game_app = std::make_unique<GameApp>(width,height);
	TestUnit::runAll();

	if(!game_app->init())
	{
		fmt::print("game app init failed\n");
		return -1;
	};

	auto time_last = std::chrono::steady_clock::now();

	//运行
	while (!glfwWindowShouldClose(window))
	{
		auto time_now = std::chrono::steady_clock::now();
		auto elapsed_millisecond_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_last).count();
		float elasped_second_time = std::max(1.0f,float(elapsed_millisecond_time)) / float(1000.0f);
		time_last = time_now;

		//fmt::print("elasped time {} s\n",elasped_second_time);
		game_app->update(elasped_second_time);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		game_app->draw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	return 0;
}
