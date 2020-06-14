#include <string>

#include "math.h"

#include "image_tracker.hpp"

using namespace std;

ImageTracker::ImageTracker(const string& name, Board * board) : Module(name, board) {
	register_memory("image", "The source image.", (Memory**)(&m_mem_imgs));
	register_memory("result", "The result of images tracking", (Memory**)(&m_mem_info3d));
}

bool ImageTracker::init_process() {
	return true;
}

bool ImageTracker::main_process() {

	Image img;
	if (!m_mem_imgs->get_data(img))
		return true;
	
	//estimate the 3d relation between recent two images.
	Info3D info_3d;

	
	//check the result of estimation of R and t.

	m_mem_info3d.set_data(info_3d);

	return  true;
}

bool ImageTracker::finish_process() {
	return true;
}

SemiDenseMapper::SemiDenseMapper(const string& name, Board * board) : Module(name, board) {
	register_memory("image", "The source image.", (Memory**)(&m_mem_imgs));
	register_memory("result", "The result of images tracking", (Memory**)(&m_mem_info_3d));
	register_memory("depth_map", "Memory of Depth maps.", (Memory**)(&m_mem_depth_maps));
}


bool SemiDenseMapper::init_process() {
	return true;
}

bool SemiDenseMapper::main_process() {


	Image img;
	m_mem_imgs->get_data(img);

	//search a matching point in a oldest image.
	m_ref_imgs.push(img);	
	queue<Info3D> que_info_3d;
	Info3D info_3d;
	m_mem_info_3d.get_data(info_3d);
	que_info_3d.push(info_3d);
	for (int i = 0; i < m_ref_imgs.size(); ++i) {
		Image ref_img = m_ref_imgs.front();

	}


	//calculate rotation matrix and traslation vector from images.	
	//Info3D info_3d;
	m_mem_info_3d.get_data(info_3d);

	Matrix3f E;
	calc_essential_matrix(info_3d.R, info_3d.t, E);

	
	//calculate the epipoler lines
	Vector3f epi_line;
	//calc_epi_line(E, epi_line);

	//search  the mathing point along the epipoler line.

	//creaate depth map

	return true;
}

bool SemiDenseMapper::finish_process() {
	return true;
}