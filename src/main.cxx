#include <iostream>

#include <GLFW/glfw3.h>

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Error in Init GLFW" << std::endl;
    }

    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        std::cout << "Error creating window" << std::endl;
    }

    unsigned char* data = new unsigned char[100 * 100 * 3];
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            data[y * 100 * 3 + x * 3    ] = 0xff;
            data[y * 100 * 3 + x * 3 + 1] = 0x00;
            data[y * 100 * 3 + x * 3 + 2] = 0x00;
        }
    }

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
        glfwSwapBuffers(window);

        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}