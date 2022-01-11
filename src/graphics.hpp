#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include "chip8.hpp"


    void framebuffer_size_callback(GLFWwindow*, int, int);
    void map_keyboard();
    GLFWwindow* setup_window(int);
    unsigned int make_VAO();
    void processInput(GLFWwindow *window, Chip8 *chip8);
        


#endif