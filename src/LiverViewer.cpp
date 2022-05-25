#include "LiverViewer.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <TextureLoader.h>

#include <test_unit.h>



int main()
{
	GLFWwindow* window = nullptr;
	if(!glfwInit())
		return -1;

	std::string title = "LiverViewer";

	window = glfwCreateWindow(800,600,title.c_str(),nullptr,nullptr);
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
	TestUnit::runAll();
	//运行
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	return 0;
}
