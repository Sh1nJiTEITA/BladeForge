#ifndef BF_UNIFORMS_H
#define BF_UNIFORMS_H

#include "bfVariative.hpp"


struct BfUniformView {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct BfObjectData {
	glm::mat4 model_matrix;
};



struct BfUniformBezierProperties {
	uint32_t points_count;
};

struct BfStorageBezierPoints {
	glm::vec2 coo;
};



#endif