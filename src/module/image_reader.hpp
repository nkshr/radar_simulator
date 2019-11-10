#pragma once

#include <ctime>

#include "../memory/memory.hpp"

#include "module.hpp"

using std::cout;
using std::endl;

class ImageReader : public Module {
protected:
	bool m_bprint;
	bool m_brev_cols;
	bool m_brev_rows;
	bool m_btranspose;

	int m_id;

	string m_path;
	string m_img_name;
	
	MemImages *m_mem_imgs;

	IMG_FMT m_img_fmt;

public:
	ImageReader(const string& name, Board * board) : Module(name, board) , m_id(0), 
	m_brev_cols(false), m_brev_rows(false){
		register_bool("print", "boolean value to spcify if print image names .(default y)",
			true, &m_bprint);
		register_string("path", "image path", "C:\\", &m_path);
		register_string("image_name", "format string representing image name", "%0x6", &m_img_name);
		//register_string("format", "Image format.", "gray", &m_format_str);
		register_memory("image", "Image Memory", (Memory**)(&m_mem_imgs));
		register_bool("rev_cols", "Reverse colums order.", false, &m_brev_cols);
		register_bool("rev_rows", "Reverse rows order.", false, &m_brev_rows);
		register_bool("transpose", "Transpose image.", false, &m_btranspose);
		register_enum<IMG_FMT>("format", "Image format(default rgb).", IFMT_RGB,
			&m_img_fmt, img_fmt_strs);
	}

	virtual bool init_process() {
		return true;
	};

	virtual bool main_process() {
		string path_and_name_format= m_path + "\\" + m_img_name;
		
		char path_and_name[1024];
		
		snprintf(path_and_name, sizeof(path_and_name),
			path_and_name_format.c_str(), m_id++);

		Image * img = nullptr;
		switch(m_img_fmt) {
		case IFMT_RGB:
		case IFMT_RGBA:
		case IFMT_GRAY:
			img = imread(string(path_and_name), IFMT_GRAY);
			break;
		}
		if (img) {
			img->set_time(get_time());

			if (m_brev_cols)
				img->reverse_cols();

			if (m_brev_rows)
				img->reverse_rows();

			if (m_btranspose)
				img->transpose();

			m_mem_imgs->set_data(img);
		}

		if (m_bprint) {
			string msg;
			if (img) {
				msg = "success : " + string(path_and_name)
					+ " " + to_string(img->get_width()) + "x"
					+ to_string(img->get_height()) + "\n";
			}
			else
				msg = "failure : " + string(path_and_name) + "\n";
			print(msg);
		}

		return true;
	};

	virtual bool finish_process() {
		return true;
	};

};