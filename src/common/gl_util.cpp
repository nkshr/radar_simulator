#include <iostream>

#include "miscel.hpp"

#include "gl_util.hpp"

using namespace std;

Shader::Shader() : vs(0), tcs(0), tes(0), gs(0), fs(0), cs(0), exe(0), vao(0)
{
}

Shader::~Shader() {
	destroy_shader(*this);
}

bool check_gl(const char *place) {

	bool ok = true;
	while (true) {
		GLenum err(glGetError());
		if (err == GL_NO_ERROR)
			break;
		ok = false;

		string err_str;
		switch (err) {
		case GL_INVALID_OPERATION:      err_str = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           err_str = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          err_str = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          err_str = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  err_str = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		cerr << "GL_" << err_str.c_str() << " at " << place << endl;
	}
	return ok;
}

int compile_shader(const char * fname, GLenum stype) {
	char *ssrc = load_text(fname);
	if (!ssrc) {
		cerr << "Error : Couldn't open " << fname << "." << endl;
		return NULL;
	}

	//cout << ssrc << endl;

	int shader = glCreateShader(stype);
	glShaderSource(shader, 1, &ssrc, NULL);
	glCompileShader(shader);
	delete ssrc;

	GLint cmp_stat;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &cmp_stat);
	if (cmp_stat == GL_FALSE) {
		cerr << "Error : Couldn't compile " << fname << "." << endl;
		int info_log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length > 0) {
			char * info_log = new char[info_log_length];
			int chars_written = 0;
			glGetShaderInfoLog(shader, info_log_length, &chars_written, info_log);
			cout << info_log << endl;
			delete info_log;
		}

		glDeleteShader(shader);
		shader = 0;
	}

	return shader;
}

bool link_shaders(Shader &shader) {
	glGenVertexArrays(1, &shader.vao);
	glBindVertexArray(shader.vao);

	shader.exe = glCreateProgram();

	if (shader.vs)
		glAttachShader(shader.exe, shader.vs);
	if (shader.tcs)
		glAttachShader(shader.exe, shader.tcs);
	if (shader.tes)
		glAttachShader(shader.exe, shader.tes);
	if (shader.gs)
		glAttachShader(shader.exe, shader.gs);
	if (shader.fs)
		glAttachShader(shader.exe, shader.fs);
	if (shader.cs)
		glAttachShader(shader.exe, shader.cs);

	glLinkProgram(shader.exe);

	int link_stat;
	glGetProgramiv(shader.exe, GL_LINK_STATUS, &link_stat);

	if (link_stat == GL_FALSE) {
		cerr << "Error : Couldn't link shader program. " << endl;
		int info_log_length;
		glGetProgramiv(shader.exe, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length > 0) {
			char * info_log = new char[info_log_length];
			int chars_written = 0;
			glGetProgramInfoLog(shader.exe, info_log_length, &chars_written, info_log);
			cerr << info_log << endl;
			delete info_log;
		}

		return false;
	}
	return true;

}

bool build_shader(Shader &shader) {
	if (!shader.vsname.empty()){
		shader.vs = compile_shader(shader.vsname.c_str(), GL_VERTEX_SHADER);
		if (!shader.vs) {
			return false;
		}
	}

	if (!shader.tcsname.empty()){
		shader.tcs = compile_shader(shader.tcsname.c_str(), GL_TESS_CONTROL_SHADER);
		if (!shader.tcs) {
			return false;
		}
	}

	if (!shader.tesname.empty()){
		shader.tes = compile_shader(shader.tesname.c_str(), GL_TESS_EVALUATION_SHADER);
		if (!shader.tes) {
			return false;
		}
	}

	if (!shader.gsname.empty()){
		shader.gs = compile_shader(shader.gsname.c_str(), GL_GEOMETRY_SHADER);
		if (!shader.gs) {
			return false;
		}
	}

	if (!shader.fsname.empty()){
		shader.fs = compile_shader(shader.fsname.c_str(), GL_FRAGMENT_SHADER);
		if (!shader.fs) {
			return false;
		}
	}

	if (!shader.csname.empty()){
		shader.cs = compile_shader(shader.csname.c_str(), GL_COMPUTE_SHADER);
		if(!shader.cs) {
			return false;
		}
	}

	return link_shaders(shader);
}

void use_shader(const Shader & shader) {
	glBindVertexArray(shader.vao);
	glUseProgram(shader.exe);
}

void destroy_shader(const Shader &shader) {
	glDetachShader(shader.exe, shader.vs);
	glDetachShader(shader.exe, shader.tcs);
	glDetachShader(shader.exe, shader.tes);
	glDetachShader(shader.exe, shader.gs);
	glDetachShader(shader.exe, shader.fs);
	glDetachShader(shader.exe, shader.cs);


	glDeleteShader(shader.vs);
	glDeleteShader(shader.tcs);
	glDeleteShader(shader.tes);
	glDeleteShader(shader.gs);
	glDeleteShader(shader.fs);
	glDeleteShader(shader.cs);

	glDeleteShader(shader.exe);

	glDeleteVertexArrays(1, &shader.vao);

}

int create_texture(Image &img) {
	GLuint object;
	
	glGenTextures(1, &object);
	glBindTexture(GL_TEXTURE_2D, object);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.get_width(), img.get_height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, img.get_pixels());
	glBindTexture(GL_TEXTURE_2D, 0);

	return object;
}

void set_1i_to_shader(const GLuint exe, const char * param, const int val) {
	GLuint loc = glGetUniformLocation(exe, param);
	glUniform1i(loc, val);
}

void set_1f_to_shader(const GLuint exe, const char * param, const float val) {
	GLuint loc = glGetUniformLocation(exe, param);
	glUniform1f(loc, val);
}

void set_3f_to_shader(const GLuint exe, const char * param,
	const float v0, const float v1, const float v2) {
	GLuint loc = glGetUniformLocation(exe, param);
	glUniform3f(loc, v0, v1, v2);
}

void set_2f_to_shader(const GLuint exe, const char * param,
	const float v0, const float v1) {
	GLuint loc = glGetUniformLocation(exe, param);
	glUniform2f(loc, v0, v1);
}

void set_mat4f_to_shader(const GLuint exe, const char * param, const float * data) {
	GLuint loc = glGetUniformLocation(exe, param);
	glUniformMatrix4fv(loc, 1, GL_FALSE, data);
}