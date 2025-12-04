#pragma once
#include"constants.hpp"
#include<SFML/Graphics.hpp>

void set_walls_dirichlet_boundary_conditions(float *Efields,  const Dimensions &dims, const float time, const float dphase_net, const int num_of_transmitter_elements, const int obstacle_fraction);
void step_time(float* Efields, const Dimensions& dims, const float csq, const float dt);

