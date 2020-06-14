#pragma once

#include <queue>


#include "../common/math.hpp"

#include "module.hpp"



class ImageTracker : public Module {
protected:
	bool m_bprint;
	MemImages * m_mem_imgs;
	MemRingBuf<Info3D> m_mem_info3d;
	Info3D m_prev_info_3d;

public:
	ImageTracker(const  string&name, Board * board);

	virtual bool init_process();
	virtual bool main_process();
	virtual bool finish_process();
};

class SemiDenseMapper : public Module {
protected:

	MemImages * m_mem_imgs;
	MemRingBuf<Info3D> m_mem_info_3d;
	MemImages * m_mem_depth_maps;

	int num_ref_imgs;

	queue<Image> m_ref_imgs;

public:
	SemiDenseMapper(const string&name, Board * board);

	virtual bool init_process();
	virtual bool main_process();
	virtual bool finish_process();
};
