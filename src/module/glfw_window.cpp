#include <iostream>
#include <Eigen/Dense>
#include <lodepng/lodepng.h>

#include "glfw_window.hpp"

using namespace std;
using namespace Eigen;
using namespace lodepng;

GLFWWindow * GLFWWindow::GLFWCallback::m_window = nullptr;

void GLFWWindow::error_callback(int error, const char * description) {
	cerr << "Error : " << description << endl;
}

void GLFWWindow::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	cout << "Info : key callback called" << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}
}

void GLFWWindow::GLFWCallback::error_callback(int error, const char * description) {
	m_window->error_callback(error, description);
}

void GLFWWindow::GLFWCallback::key_callback(GLFWwindow * window, int key, int scancode,
	int action, int mods) {
	m_window->key_callback(window, key, scancode, action, mods);
}

void GLFWWindow::GLFWCallback::set_window(GLFWWindow * window) {
	m_window = window;
}

GLFWWindow::GLFWWindow(const string& name, Board * board) : Module(name, board), m_glfw_initialized(false),
m_width(640), m_height(480), m_ratio(640.f / 480.f), m_name("glfw window"),
m_brev_rows(false), m_brev_cols(false), m_bpause(false),
m_brev_rows_done(false), m_brev_cols_done(false){
	register_bool("rev_rows", "Reverse rows order", false, &m_brev_rows);
	register_bool("rev_cols", "Reverse cols order", false, &m_brev_cols);
	register_bool("pause", "Pause", false, &m_bpause);
}

bool GLFWWindow::init_process() {
	if (!m_glfw_initialized) {
		if (!glfwInit()) {
			cerr << "Error : GLFW initialization was failed." << endl;
			return false;
		}
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	m_window = glfwCreateWindow(m_width, m_height, m_name.c_str() , NULL, NULL);

	GLFWWindow::GLFWCallback::set_window(this);

	glfwSetErrorCallback(GLFWWindow::GLFWCallback::error_callback);
	glfwSetKeyCallback(m_window, GLFWWindow::GLFWCallback::key_callback);

	if (!m_window) {
		cerr << "Error : Creation of GLFW window was failed." << endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cerr << "Error : Initialization of GLEW was failed.\n" 
			<< glewGetErrorString(err) << endl;
		return false;
	}

	glfwSwapInterval(-1);

	build_shader(m_shader);

	return true;
}

bool GLFWWindow::main_process(){
	if (glfwWindowShouldClose(m_window))
		return false;	

	glViewport(0, 0, m_width, m_height);

	glClearColor(1.f, 0.f, 0.f, 0.5f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glfwSwapBuffers(m_window);

	glfwPollEvents();

	return true;
}

bool GLFWWindow::finish_process() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
	return true;
}

SLAMViewer::SLAMViewer(const string&name, Board * board) : GLFWWindow(name, board), m_bprint(false), m_binitialized(false), m_img(nullptr){
	m_shader.vsname = "C:/cygwin64/home/naoka/github/radar_simulator/src/shader/texture.vs";
	m_shader.fsname = "C:/cygwin64/home/naoka/github/radar_simulator/src/shader/texture.fs";

	register_int("width", "Window width", 1241, &m_width);
	register_int("height", "Window height", 376, &m_height);
	register_memory("image", "Source Image for SLAM", (Memory**)(&m_mem_img));
	register_bool("print", "Flag to print information.", false, &m_bprint);
}

bool SLAMViewer::init_process() {
	m_binitialized = true;
	if (!GLFWWindow::init_process())
		return false;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	GLfloat vertices[] = {
		1.0f,  1.0f, 1.0f, 1.0f, //top right
		1.0f, -1.0f, 1.0f, 0.0f, //bottom right
		-1.0f, -1.0f, 0.0f, 0.0f, //bottom left
		-1.0f,  1.0f, 0.0f, 1.0f, //top left
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLint pos_attrib_loc = glGetAttribLocation(m_shader.exe, "pos");
	glEnableVertexAttribArray(pos_attrib_loc);
	glVertexAttribPointer(pos_attrib_loc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attrib_loc);

	GLint tex_coord_attrib_loc = glGetAttribLocation(m_shader.exe, "tex_coord");
	glEnableVertexAttribArray(tex_coord_attrib_loc);
	glVertexAttribPointer(tex_coord_attrib_loc, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(tex_coord_attrib_loc);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	GLint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenTextures(1, &m_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	set_1i_to_shader(m_shader.exe, "tex", 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return true;
}

bool SLAMViewer::main_process() {
	if (glfwWindowShouldClose(m_window))
		return false;

	glViewport(0, 0, m_width, m_height);

	glClearColor(1.f, 0.f, 0.f, 0.5f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_mem_img) {
		if (!m_bpause) {
			delete m_img;
			m_img = m_mem_img->get_data();
			m_brev_rows_done = m_brev_cols_done = false;
		}
		
		if (m_bprint && m_img) {
			string msg = "time : " + to_string(m_img->get_time()) + '\n';
			print(msg);
		};
	}

	if (m_img) {
		if ((m_brev_rows &&  !m_brev_rows_done) || (!m_brev_rows && m_brev_rows_done)) {
			m_img->reverse_rows();
			m_brev_rows_done = !m_brev_rows_done;

		}

		if (m_brev_cols && !m_brev_cols_done || (!m_brev_cols && m_brev_cols_done)) {
			m_img->reverse_cols();
			m_brev_cols_done = !m_brev_cols_done;
		}

		int w = m_img->get_width();
		int h = m_img->get_height();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			m_img->get_width(), m_img->get_height(), 0,
			GL_RED, GL_UNSIGNED_BYTE, m_img->get_pixels());
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		//	m_img->get_width(), m_img->get_height(), 0,
		//	GL_RGBA,GL_UNSIGNED_BYTE, m_img->get_pixels());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glUseProgram(m_shader.exe);

	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glfwSwapBuffers(m_window);

	glfwPollEvents();

	return true;
}

bool SLAMViewer::finish_process() {

	return true;
}