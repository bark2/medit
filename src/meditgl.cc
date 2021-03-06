#include <iostream>
#include <cctype>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include "types.hh"
#include "text_renderer.hh"
#include "buffer.hh"


const GLuint WIDTH = 800, HEIGHT = 600;
const GLuint font_size = 20;
const GLuint rows = HEIGHT/font_size;



TextRenderer textRenderer;
Buffer buffer;

enum Mode {
    INSERT_MODE = 0,
    COMMAND_MODE,
    MARK_MODE
};
static Mode mode;

// todo: later to a keymapping option, reading a config file mb
// todo: read more about openGL
void key_callback(GLFWwindow* window, int key, int scancod, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
	if (mode == INSERT_MODE)
	    switch (key) {
	    case GLFW_KEY_ENTER:
		buffer.emplace_line();
		break;
	    default:
		if (key < 97) {
		    if (key == GLFW_KEY_SPACE && (mods && GLFW_MOD_ALT)) {
			mode = COMMAND_MODE;
			break;
		    }
		    if (mods & GLFW_MOD_SHIFT) {
			buffer.emplace_char(key);
		    } else {
			buffer.emplace_char(tolower(key));
		    }
		    break;
		}
	    } else if (mode == COMMAND_MODE) {
	    switch(key) {
	    case GLFW_KEY_ENTER:
		buffer.emplace_line();
		break;
	    case 'D':
		buffer.erase_char();
		break;
	    case 'L':
		buffer.cursor_next();
		break;
	    case 'J':
		buffer.cursor_prev();
		break;
	    case 'I':
		buffer.cursor_up();
		break;
	    case 'K':
		buffer.cursor_down();
		break;
	    case 'F':
		mode = INSERT_MODE;
		break; 
	    case 'Q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	    }
	}
    }
}

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "medit", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
	std::cout << "Failed to initialize OpenGL context" << std::endl;
	return -1;
    }
    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    textRenderer.Init(WIDTH, HEIGHT);
    textRenderer.Load("Inconsolata-Regular.ttf", font_size);
    
    glfwSetKeyCallback(window, key_callback);
    
    while (!glfwWindowShouldClose(window)) {
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
		
	// Check and call events
	glfwWaitEvents();
	// text rendering
	textRenderer.render_buffer(buffer.lines.begin(),
				   buffer.lines.end(),
				   buffer.cline,
				   buffer.ccursor(),
				   1.0f,
				   glm::vec3(255.0f, 255.0f, 255.0f));
	// Swap the buffers
	glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

