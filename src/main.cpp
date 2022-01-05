#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "chip8.hpp"
#include "platform.hpp"
#include <chrono>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main(int argc, char** argv)
{
	// if (argc != 4)
	// {
	// 	std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
	// 	std::exit(EXIT_FAILURE);
	// }

	// int videoScale = std::stoi(argv[1]);
	// int cycleDelay = std::stoi(argv[2]);
	// char const* romFilename = argv[3];
	int videoScale = 20;

	// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(64 * videoScale, 32 * videoScale, "Chip-8 With Shaders", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

	// Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

	// Chip8 chip8;
	// chip8.loadROM(romFilename);

	// int videoPitch = sizeof(chip8.screen[0]) * VIDEO_WIDTH;

	// auto lastCycleTime = std::chrono::high_resolution_clock::now();
	// bool quit = false;

	// while (!quit)
	// {
	// 	quit = platform.ProcessInput(chip8.key);

	// 	auto currentTime = std::chrono::high_resolution_clock::now();
	// 	float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

	// 	if (dt > cycleDelay)
	// 	{
	// 		lastCycleTime = currentTime;

	// 		chip8.cycle();

	// 		platform.Update(chip8.screen, videoPitch);
	// 	}
	// }

	// return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}