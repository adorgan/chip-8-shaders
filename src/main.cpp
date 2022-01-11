#include <glad/glad.h>
#include <GLFW/glfw3.h>
// OpenGL math libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "chip8.hpp"
#include "Shader.h"
#include "graphics.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Chip8 *chip8);

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <Cycle Dela> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

    

	// int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[1]);
	char const* romFilename = argv[2];
	int videoScale = 30;
    
    // create window
    GLFWwindow* window = setup_window(30);

	// build and compile our shader program
    // ------------------------------------
    Shader ourShader("shaders/shader.vert", "shaders/shader.frag"); // you can name your shader files however you like

    // create VAO for rendering
    unsigned int VAO = make_VAO();

    unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    unsigned int colorLoc = glGetUniformLocation(ourShader.ID, "color");
    glm::vec3 color;

    glfwSwapInterval(0);

    // load chip 8
    Chip8 *chip8 = new Chip8();
	chip8->loadROM(romFilename);

    // render loop
    // -----------
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, chip8);

        ourShader.use();
        glBindVertexArray(VAO);
        
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        GLfloat color[3]; 
        
        auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        printf("%f\n", dt);
		if (dt > cycleDelay)
		{
            
			lastCycleTime = currentTime;

			// chip 8 cycle
            chip8->cycle();
            for(int y = 0; y < VIDEO_HEIGHT; y++){
                for(int x = 0; x < VIDEO_WIDTH; x++){
                    if(chip8->screen[(y * VIDEO_WIDTH) + x] == 0){
                        color[0] = 0.0f;
                        color[1] = 0.0f;
                        color[2] = 0.0f;
                        
                    }
                    else{
                        color[0] = 1.0f;
                        color[1] = 1.0f;
                        color[2] = 1.0f;
                        
                    }
                    float x_coord = x / (64.0f);
                    float y_coord = -y / (32.0f);

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(x_coord, y_coord, 0.0f));
                    model = glm::translate(model, glm::vec3(-.5f, .5f, 0.0f));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform3fv(colorLoc, 1, color);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }
		} 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

}


