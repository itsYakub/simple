#include "../simple.h"

#include <math.h>
#include <stdio.h>
#include <glad/gl.h>
#include <GL/gl.h>
#include <GL/glx.h>

static const GLchar	*___shader_vertex =
"#version 330 core\n"
"layout (location = 0) in vec3	a_pos;\n"
"layout (location = 1) in vec4	a_col;\n"
"out vec4						v_col;\n"
"\n"
"void main() {\n"
"	gl_Position = vec4(a_pos, 1.0);\n"
"	v_col = a_col;\n"
"}\n";

static const GLchar	*___shader_fragment =
"#version 330 core\n"
"in vec4	v_col;\n"
"out vec4	f_col;\n"
"\n"
"void main() {\n"
"	f_col = v_col;\n"
"}\n";


int main(void) {
	GLuint	_shader;
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;
	
	init(800, 600, "OpenGL - X11 - simple");
	gladLoadGLLoader((GLADloadproc) glXGetProcAddress);

	/* Creating OpenGL shader objects */
	{
		GLuint	_shader_vertex;
		GLuint	_shader_fragment;

		_shader_vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(_shader_vertex, 1, &___shader_vertex, 0);
		glCompileShader(_shader_vertex);
		_shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(_shader_fragment, 1, &___shader_fragment, 0);
		glCompileShader(_shader_fragment);

		_shader = glCreateProgram();
		glAttachShader(_shader, _shader_vertex);
		glAttachShader(_shader, _shader_fragment);
		glLinkProgram(_shader);
		glDeleteShader(_shader_vertex);
		glDeleteShader(_shader_fragment);
	}

	/* Creating OpenGL memory objects */
	{
		const GLfloat	_vertices[] = {
			-0.6f, -0.75f, 0.0f,	1.0f, 0.0f, 0.0f, 1.0f,
			 0.6f, -0.75f, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f,
			 0.0,   0.75f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f
		};
		const GLuint	_indices[] = {
			0, 1, 2
		};

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *) (0 * sizeof(GLfloat)));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	while (!should_quit()) {
		clear(0.2f, 0.2f, 0.2f, 1.0f);

		glUseProgram(_shader);
		glBindVertexArray(_vao);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		display();
		poll_events();
	}
	
	glDeleteProgram(_shader);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao);

	quit();
}
