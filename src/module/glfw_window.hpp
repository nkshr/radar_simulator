#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Eigen/Dense>

#include "../common/gl_util.hpp"

#include "module.hpp"

using std::string;


class GLFWWindow : public Module {
protected:
	GLFWwindow * m_window;

	Shader m_shader;

	bool m_glfw_initialized;

	int m_width;
	int m_height;
	
	float m_ratio;

	string m_name;

	bool m_brev_rows;
	bool m_brev_cols;
	bool m_bpause;

	bool m_brev_rows_done;
	bool m_brev_cols_done;

	virtual	void error_callback(int error, const char * description);
	virtual void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);

	class GLFWCallback {
	public:
		static void error_callback(int error, const char * description);
		static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);

		static void set_window(GLFWWindow * window);

	private:
		static GLFWWindow * m_window;
	};

public:
	GLFWWindow(Board * board);
	
	virtual bool init_process();
	virtual bool main_process();
	virtual bool finish_process();

};


class SLAMViewer : public GLFWWindow {
private:
	GLuint m_frame_buf;
	GLuint m_src_img_tex;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_tex;
	
	MemImage * m_mem_img;
	
	Image * m_img;

	bool m_bprint;

	bool m_binitialized;

public:
	SLAMViewer(Board * board);

	virtual bool init_process();
	virtual bool main_process();
	virtual bool finish_process();

};