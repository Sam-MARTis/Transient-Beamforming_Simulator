#pragma once
#include"constants.hpp"
#include<SFML/Graphics.hpp>

void set_walls_dirichlet_boundary_conditions(float *EfieldsX, float *EfieldsY, const Dimensions &dims, const float time, const float dphase_net, const int num_of_transmitter_elements, const float obstacle_fraction, const float frequency_transmitter);
void step_time(float* EfieldsX, float* EfieldsY, const Dimensions& dims, const float csq, const float dt);
void get_field_strength_magnitude(const float* EfieldsX, const float* EfieldsY, const Dimensions& dims, float* magnitude_out);

