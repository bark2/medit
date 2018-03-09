#include <iostream>
#include <cctype>

#include <codecvt>
#include <locale>
#include <unistd.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.hh"
#include "gap_buffer.hh"

const GLuint WIDTH = 800, HEIGHT = 600;
const GLuint font_size = 20;
const GLuint rows = HEIGHT/font_size;


TextRenderer textRenderer;
PieceTable piece_table;

void key_callback(GLFWwindow* window, int key, int scancod, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
	switch (key) {
	case GLFW_KEY_ENTER:
	    piece_table.insertPiece();
	case -1: // UNKOWN_KEY
	    break;
	case 'D':
	    if (mods & GLFW_MOD_ALT) {
		piece_table.backspace();
		break;
	    }
	case 'Q':
	    if (mods & GLFW_MOD_ALT) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	    }
	default:
	    if (key < 97) {
		if (mods & GLFW_MOD_SHIFT)
		    piece_table.insert(key);
		else
		    piece_table.insert(tolower(key));
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	PieceTable::NextWritable next_writable = PieceTable::NextWritable(piece_table.pieces.cbegin());
	textRenderer.RenderText(piece_table.pieces.cbegin(),
				piece_table.pieces.cend(),
				next_writable,
				0.0f,
				0.0f,
				1.0f,
				glm::vec3(255.0f, 255.0f, 255.0f));
	
	// Swap the buffers
	glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

