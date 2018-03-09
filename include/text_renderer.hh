/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <list>
#include <functional>
#include <utility>

#include "glad/glad.h"
#include <glm/glm.hpp>

//#include "texture.h"
#include "shader.hh"
#include "gap_buffer.hh"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
    // Holds a list of pre-compiled Characters
    std::map<GLchar, Character> Characters;
    // Shader used for text rendering
    Shader TextShader;
    // Constructor
    TextRenderer() = default;
    void Init(GLuint width, GLuint height);
    // Pre-compiles a list of characters from the given font
    void Load(std::string font, GLuint fontSize);
    // Renders a string of text using the precompiled list of characters

    void RenderText(std::list<Line>::iterator start_row,
		    std::list<Line>::iterator end_row,
		    coord cursor,
		    GLfloat x,
		    GLfloat y,
		    GLfloat scale,
		    glm::vec3 color)
    {
	// Activate corresponding render state	
	this->TextShader.Use();
	this->TextShader.SetVector3f("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	GLfloat glyph_size;
	int row = 0;
	for (auto cr = start_row; cr != end_row; cr++) {
	    int col = 0;
	    char* c;
	    Style* style;
	    cr->init_iterating();
	    while (cr->next_char(&c, &style)) {
		Character ch = Characters[*c];
		
		glyph_size = ch.Size;
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
		    { xpos,     ypos + h,   0.0, 1.0 },
		    { xpos + w, ypos,       1.0, 0.0 },
		    { xpos,     ypos,       0.0, 0.0 },

		    { xpos,     ypos + h,   0.0, 1.0 },
		    { xpos + w, ypos + h,   1.0, 1.0 },
		    { xpos + w, ypos,       1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		col++;
	    }
	    x = 0;
	    y += glyph_size;
	    row++;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
    }
    void RenderChar(unsigned char u8, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));

private:
    // Render state
    GLuint VAO, VBO;
};

#endif 

