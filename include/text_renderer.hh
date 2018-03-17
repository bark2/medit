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
#include "types.hh"

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
    int cfontSize;
    // Holds a list of pre-compiled Characters
    std::map<GLchar, Character> Characters;
    // Shader used for text rendering
    Shader TextShader;
    Shader cursor_shader;
    // Constructor
    TextRenderer() = default;
    void Init(GLuint width, GLuint height);
    // Pre-compiles a list of characters from the given font
    void Load(std::string font, GLuint fontSize);
    // Renders a string of text using the precompiled list of characters

    void RenderChar(unsigned char u8, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    void render_buffer(std::list<Line>::iterator start_row,
		      std::list<Line>::iterator end_row,
		      GLfloat x,
		      GLfloat y,
		      GLfloat scale,
		      glm::vec3 color);
private:
    // Render state
    GLuint VAO, VBO;
    GLuint cursor_vao, cursor_vbo;
};

#endif 

