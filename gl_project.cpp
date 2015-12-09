// This very simple example illustrates the concept of a framebuffer
// We do not really need any libraries or hardware to do computer graphics
// In this case the framebuffer is simply saved as a raw RGB and TGA image

#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tga.h"

#define byte unsigned char
#define MAX 255
#define KERNEL_FILE "kernel.txt"

#define SIZE 512

using namespace std;

#include "functions.h"
#include "IMG.h"
#include "alpha.h"
#include "shader.h"

IMAGE lena;
IMAGE image;
AIMG wow;
AIMG doge;


Point a, b, c, d;



void draw_r(){
    IMG_draw_line_bresenham(lena, 100, 100, 200, 100);
    IMG_draw_line_bresenham(lena, 100, 100, 100, 400);
    IMG_draw_line_bresenham(lena, 100, 400, 150, 400);
    IMG_draw_line_bresenham(lena, 150, 250, 150, 400);
    IMG_draw_line_bresenham(lena, 200, 400, 250, 400);
    IMG_draw_line_bresenham(lena, 150, 125, 175, 125);
    IMG_draw_line_bresenham(lena, 150, 225, 175, 225);
    IMG_draw_line_bresenham(lena, 150, 125, 150, 225);
    //obluk R
    a.y = 100;
    a.x = 200;
    b.y = 100;
    b.x = 300;
    c.y = 250;
    c.x = 300;
    d.y = 250;
    d.x = 200;
    IMG_draw_curve(lena, a, b, c, d);
    // koniec obluk R
    //noha R vnutorna
    a.y = 250;
    a.x = 150;
    b.y = 250;
    b.x = 200;
    c.y = 400;
    c.x = 200;
    d.y = 400;
    d.x = 200;
    IMG_draw_curve(lena, a, b, c, d);
    // koniec noha R vnutorna
    //noha R vonkajsia
    a.y = 250;
    a.x = 200;
    b.y = 250;
    b.x = 250;
    c.y = 400;
    c.x = 250;
    d.y = 400;
    d.x = 250;
    IMG_draw_curve(lena, a, b, c, d);
    // koniec noha R vonkajsia
    //vnutro R polkruhu
    a.y = 125;
    a.x = 175;
    b.y = 125;
    b.x = 275;
    c.y = 225;
    c.x = 275;
    d.y = 225;
    d.x = 175;
    IMG_draw_curve(lena, a, b, c, d);
}


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
    //printf("%d, %d\n", (int)xpos, (int)ypos);
    a.x = 0;
    a.y = 500;

    b.x = -100;
    b.y = -100;

    c.x = 500;
    c.y = 10;

    d.x = 500;
    d.y = 500;

    Point p1;
    p1 = IMG_get_bezier(a, b, c, d, xpos / 512.0);
    IMG_copy_over(lena, image);
    IMG_draw_line_bresenham(image, 10, 10, 10, 400);
    IMG_draw_line_bresenham(image, 10, 10, 400, 10);
    IMG_draw_line_bresenham(image, 0, 0, 400, 511);
    A_merge_RGB_RGBA(image, doge, xpos - 200, ypos - 200);

}

int main() {

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return EXIT_FAILURE;
    }

    // Setup OpenGL context
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Try to create a window
    auto window = glfwCreateWindow( SIZE, SIZE, "OpenGL Projekt - Matej Bellus", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Finalize window setup
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    if (!glewIsSupported("GL_VERSION_3_3")) {
        std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Load shaders
    auto program_id = ShaderProgram("gl_texture.vert", "gl_texture.frag");
    glUseProgram(program_id);

    InitializeGeometry(program_id);

    // Load and bind texture
    auto texture_id = LoadImage("lena.rgb", SIZE, SIZE);
    auto texture_attrib = glGetUniformLocation(program_id, "Texture");
    glUniform1i(texture_attrib, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);


    // Task1: Load RAW image file here instead
    image = IMG_load_raw(SIZE, SIZE, "lena.rgb");
    lena  = IMG_load_raw(SIZE, SIZE, "lena.rgb");

    // edge detection
    int filter_size;
    double ** filter = load_kernel(KERNEL_FILE, &filter_size);

    // Task2: Apply a convolution filter to the loaded image
    IMG_apply_kernel(image, filter, filter_size);


    // Task3: Merge multiple images using alpha blending
    // Task4: Draw lines and Bezier curves
    IMG_draw_line_bresenham(image, 400, 511, 0, 0);


    IMG_draw_curve(image, a, b, c, d);

    doge = A_load_raw(200, 200, "alpha.raw");
    wow  = A_load_raw(200, 200, "wow.raw");
    A_merge_RGB_RGBA(image, doge, 100, 100);
    A_merge_RGB_RGBA(image, wow, 100, 100);
    IMG_draw_curve(lena, a, b, c, d);
    draw_r();




 // Helper object to save the framebuffer as TGA image
    TGAImage tga(SIZE, SIZE, 3, image->image);
    tga.write_tga_file("result.tga", true);

    glfwSetCursorPosCallback(window, cursor_position_callback);

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, image->image);

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();




    return 0;
}
