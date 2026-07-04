#include <stdio.h>

#include <GLFW/glfw3.h>

#include "video_reader.hpp"


int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        printf("Error in GLFW Init\n");
        return 1;
    }

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        printf("Error creating window");
        return 1;
    }

    VideoReaderState vr_state;
    if (!video_reader_open(&vr_state, "/mnt/c/Users/ILYA/Desktop/Wednesday3.mp4")) {
        printf("Could not open file\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    // generate texture
    GLuint tex_handle;
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // allocate frame buffer
    const int frame_width = vr_state.width;
    const int frame_height = vr_state.height;
    uint8_t* frame_data = new uint8_t[frame_width * frame_height * 4];

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set up orthographic projection
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_width, window_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        // read a new frame and load it into the texture
        if (!video_reader_read_frame(&vr_state, frame_data)) {
            printf("Could not load video frame\n");
            return 1;
        }
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);

        // render whatever you want
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(0, 0);
            glTexCoord2d(1,0); glVertex2i(frame_width, 0);
            glTexCoord2d(1,1); glVertex2i(frame_width, frame_height);
            glTexCoord2d(0,1); glVertex2i(0, frame_height);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    video_reader_close(&vr_state);

    return 0;
}