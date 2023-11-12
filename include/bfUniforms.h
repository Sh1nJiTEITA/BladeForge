#ifndef BF_UNIFORMS_H
#define BF_UNIFORMS_H

#include "bfVariative.hpp"


struct BfViewUniform {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};



#endif