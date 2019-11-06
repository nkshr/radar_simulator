#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../common/miscel.hpp"

using std::string;

struct Shader {
	Shader();
	~Shader();

	GLuint vs;
	GLuint tcs;
	GLuint tes;
	GLuint gs;
	GLuint fs;
	GLuint cs;
	GLuint exe;
	GLuint vao;

	string vsname;
	string tcsname;
	string tesname;
	string gsname;
	string fsname;
	string csname;
};

bool check_gl(const char *place);

int compile_shader(const char * fname, GLenum stype);

bool link_shaders(Shader &shader);

bool build_shader(Shader &shader);

void use_shader(const Shader&shader);

void destroy_shader(const Shader &shader);

int create_texture(Image &img);

void set_1i_to_shader(const GLuint exe, const char * param, const int val);

void set_1f_to_shader(const GLuint exe, const char * param, const float val);

void set_3f_to_shader(const GLuint exe, const char * param,
	const float v0, const float v1, const float v2);

void set_mat4f_to_shader(const GLuint exe, const char * param, const float * data);

