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

#define  SCREEN_WIDTH  64
#define  SCREEN_HEIGHT 32

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

	// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH * videoScale, SCREEN_HEIGHT * videoScale, "Chip-8 With Shaders", NULL, NULL);
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

	// build and compile our shader program
    // ------------------------------------
    Shader ourShader("shaders/shader.vert", "shaders/shader.frag"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         1.0f/(SCREEN_WIDTH * 1),  1.0f/(SCREEN_HEIGHT * 1), 0.0f,  // top right
         1.0f/(SCREEN_WIDTH * 1), -1.0f/(SCREEN_HEIGHT * 1), 0.0f,  // bottom right
        -1.0f/(SCREEN_WIDTH * 1), -1.0f/(SCREEN_HEIGHT * 1), 0.0f,  // bottom left
        -1.0f/(SCREEN_WIDTH * 1),  1.0f/(SCREEN_HEIGHT * 1), 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

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
            for(int y = 0; y < SCREEN_HEIGHT; y++){
                for(int x = 0; x < SCREEN_WIDTH; x++){
                    if(chip8->screen[(y * SCREEN_WIDTH) + x] == 0){
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


// process all input: 
// 1 2 3 4
// Q W E R
// A S D F
// Z X C V
void processInput(GLFWwindow *window, Chip8 *chip8)
{
    
    // 1 2 3 4
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        chip8->key[0x0] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE){
        chip8->key[0x0] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        chip8->key[0x1] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE){
        chip8->key[0x1] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
        chip8->key[0x2] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE){
        chip8->key[0x2] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
        chip8->key[0x3] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE){
        chip8->key[0x3] = 0;
    }

    // Q W E R
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        chip8->key[0x4] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE){
        chip8->key[0x4] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        chip8->key[0x5] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE){
        chip8->key[0x5] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        chip8->key[0x6] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE){
        chip8->key[0x6] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        chip8->key[0x7] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE){
        chip8->key[0x7] = 0;
    }

    // A S D F
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        chip8->key[0x8] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE){
        chip8->key[0x8] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        chip8->key[0x9] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE){
        chip8->key[0x9] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        chip8->key[0xA] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE){
        chip8->key[0xA] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        chip8->key[0xB] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE){
        chip8->key[0xB] = 0;
    }

    // Z X C V
    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        chip8->key[0xC] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE){
        chip8->key[0xC] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        chip8->key[0xD] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE){
        chip8->key[0xD] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
        chip8->key[0xE] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE){
        chip8->key[0xE] = 0;
    }

    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
        chip8->key[0xF] = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE){
        chip8->key[0xF] = 0;
    }

    // QUIT
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