#ifndef BF_UNIFORMS_H
#define BF_UNIFORMS_H

#include "bfVariative.hpp"


struct BfUniformView {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct BfObjectData {
	alignas(16) glm::mat4 model_matrix;
	uint16_t id;
	
	uint32_t __assign_id() {
		static uint32_t next_id = 0;
		next_id++;
		return next_id;
	}
};



struct BfUniformBezierProperties {
	uint32_t points_count;
};

struct BfStorageBezierPoints {
	glm::vec2 coo;
};



#endif