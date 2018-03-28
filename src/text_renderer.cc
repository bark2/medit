
/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.hh"

extern Coord cursor;

void TextRenderer::Init(GLuint width, GLuint height)
{
    // Load and configure shader
    this->TextShader.Compile("../src/shaders/text.vs", "../src/shaders/text.frag");
    this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f), GL_TRUE);
    this->TextShader.SetInteger("text", 0);
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    cursor_shader.Compile("../src/shaders/cursor.vs", "../src/shaders/cursor.frag");
    cursor_shader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f), GL_TRUE);
    cursor_shader.SetVector3f("cursor_color", glm::vec3(0.0f, 0.0f, 0.0f), GL_TRUE);
    glGenVertexArrays(1, &cursor_vao);
    glGenBuffers(1, &cursor_vbo);
    glBindVertexArray(cursor_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, GLuint fontSize)
{
    cfontSize = fontSize;
    min_adv_x = 1;
    adv_y = fontSize;
    // First clear the previously loaded Characters
    this->Characters.clear();
    // Then initialize and load the FreeType library
    FT_Library ft;    
    if (FT_Init_FreeType(&ft)) // All functions return a value different than 0 whenever an error occurred
	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    // Load font as face
    FT_Face face;
    font = "../fonts/" + font;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
    // Then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; c++) {
	// Load character glyph 
	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
	    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	    continue;
	}
	// Generate texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		     GL_TEXTURE_2D,
		     0,
		     GL_RED,
		     face->glyph->bitmap.width,
		     face->glyph->bitmap.rows,
		     0,
		     GL_RED,
		     GL_UNSIGNED_BYTE,
		     face->glyph->bitmap.buffer
		     );
	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       
	// Now store character for later use
	Character character = {
	    texture,
	    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
	    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
	    face->glyph->advance.x
	};
	Characters.insert(std::pair<GLchar, Character>(c, character));
	// if (face->glyph->bitmap_left < min_adv_x) {
	    // std::cout << face->glyph->bitmap_left << '\n';
	    // min_adv_x = face->glyph->bitmap_left;
	// }
	// if (face->glyph->advance.y > adv_y) {
	    // assert(0);
	    // std::cout << face->glyph->advance.y << '\n';
	    // adv_y = face->glyph->advance.y;
	// }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


void TextRenderer::RenderChar(unsigned char u8, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // Iterate through all characters
    Character ch = Characters[u8];

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
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::render_buffer(std::list<Line>::iterator start_line,
				 std::list<Line>::iterator end_line,
				 std::list<Line>::iterator cline,
				 uint32_t ccursor,
				 GLfloat scale,
				 glm::vec3 color)
{
    // Activate corresponding render state	
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    GLfloat glyph_size;
    int x = 0;
    int y = 0;
    for (auto cl = start_line; cl != end_line; cl++) {
	if (cl->nchars) {
	    uint32_t i = cl->start_offset();
	    do {
		Character ch = Characters[cl->line[i]];
		
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
	    
		// Cursor Rendering
		// update to match the scale ..  
		if (cl == cline && i+1 == ccursor) {
		    // assert(0);
		    // mb render 'ascii 219' with proper glBlendingFunc
		    GLfloat cursor_x = xpos + (ch.Advance >> 6) * scale;
		    GLfloat cursor_vertices[6][2] = {
			{cursor_x, ypos},
			{cursor_x + min_adv_x, ypos},
			{cursor_x, ypos + adv_y},
		
			{cursor_x + min_adv_x, ypos},
			{cursor_x, ypos + adv_y},
			{cursor_x + min_adv_x, ypos + adv_y}
		    };
		    // GLfloat cursor_vertices[6][2] = {
		    // {200, 200},
		    // {200, 400},
		    // {400, 400},
		
		    // {0, 0},
		    // {0, 0},
		    // {0, 0}
		    // };
		
		    cursor_shader.Use();
		    glBindVertexArray(cursor_vao);
		    glBindBuffer(GL_ARRAY_BUFFER, cursor_vbo);
		    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cursor_vertices), cursor_vertices);
		    glBindBuffer(GL_ARRAY_BUFFER, 0);
		    glDrawArrays(GL_TRIANGLES, 0, 6);
		
		    glBindBuffer(GL_ARRAY_BUFFER, 0);
		    this->TextShader.Use();
		    this->TextShader.SetVector3f("textColor", color);
		    glActiveTexture(GL_TEXTURE0);
		    glBindVertexArray(this->VAO);
    		
		} 
		// Now advance cursors for next glyph
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
	    } while(cl->next_from_offset(i, i));
	}
	x = 0;
	y += cfontSize;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
