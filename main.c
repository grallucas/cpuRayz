#include <GLFW/glfw3.h>
#include "env.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	
	glfwSwapInterval(1);
	
	int gltexture;
	
	glGenTextures(1, &gltexture);
	glBindTexture(GL_TEXTURE_2D, gltexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); //GL_lINEAR for bluring
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
	
	init();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveCamera(mcForward);
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveCamera(mcBackward);
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveCamera(mcLeft);
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveCamera(mcRight);
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) moveCamera(mcUp);
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) moveCamera(mcDown);

        if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) moveObject(mcForward);
        if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) moveObject(mcBackward);
        if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) moveObject(mcLeft);
        if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) moveObject(mcRight);
        if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) moveObject(mcUp);
        if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) moveObject(mcDown);

        update();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
		
		draw();
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, img);
	
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
		glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}