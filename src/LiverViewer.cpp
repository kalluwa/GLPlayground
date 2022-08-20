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

void calc_FPS(GLFWwindow* window,float& dt)
{
	static float t0 = 0.0;
	static float t1 = 0.0;
	static char title[256];
	static int frames = 0;

	float t = (float)glfwGetTime();

	dt = t - t0;
	t0 = t;

	t1 += dt;

	if (t1 > 0.25)
	{
		float fps = (float)frames / t1;

		GLuint64 timeStamp[7];

		//for (int i = 0; i < 7; ++i)
		//	glGetQueryObjectui64v(queryID[i], GL_QUERY_RESULT, &timeStamp[i]);

		//double geom = (timeStamp[1] - timeStamp[0]) / 1000000.0;
		//double down = (timeStamp[2] - timeStamp[1]) / 1000000.0;
		//double ao = (timeStamp[3] - timeStamp[2]) / 1000000.0;
		//double up = (timeStamp[4] - timeStamp[3]) / 1000000.0;
		//double blur = (timeStamp[5] - timeStamp[4]) / 1000000.0;
		//double comp = (timeStamp[6] - timeStamp[5]) / 1000000.0;
		//double tot = (timeStamp[6] - timeStamp[0]) / 1000000.0;

		//sprintf(title, "Fullres: %i, FPS: %2.1f, time(ms): geom %2.2f, down %2.2f, ao %2.2f, up %2.2f, blur %2.2f, comp %2.2f tot %2.2f",
		//	(int)fullres,
		//	fps,
		//	geom,
		//	down,
		//	ao,
		//	up,
		//	blur,
		//	comp,
		//	tot);

		sprintf(title,"FPS:%2.1f",fps);
		glfwSetWindowTitle(window, title);
		t1 = 0.0;
		frames = 0;
	}

	++frames;
}

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

void window_size_callback(GLFWwindow* window, int width, int height)
{
	game_app->resize(width,height);
}
int main()
{
	GLFWwindow* window = nullptr;
	if(!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::string title = "LiverViewer";

	int width = 800;
	int height = 600;
	//glfwWindowHint(GLFW_SAMPLES, 4);
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
	glfwSetWindowSizeCallback(window, window_size_callback);
	//初始化实例
	game_app = std::make_unique<GameApp>(width,height);
	TestUnit::runAll();

	if(!game_app->init())
	{
		fmt::print("game app init failed\n");
		return -1;
	};

	auto time_last = std::chrono::steady_clock::now();


	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1,1,1,1);
	float dt;
	//运行
	while (!glfwWindowShouldClose(window))
	{
		//auto time_now = std::chrono::steady_clock::now();
		//auto elapsed_millisecond_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_last).count();
		//float elasped_second_time = std::max(1.0f,float(elapsed_millisecond_time)) / float(1000.0f);
		//time_last = time_now;

		calc_FPS(window,dt);

		//fmt::print("elasped time {} s\n",elasped_second_time);
		game_app->update(dt);//elasped_second_time);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		game_app->draw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	return 0;
}
