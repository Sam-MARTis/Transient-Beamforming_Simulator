#pragma once
#include"constants.hpp"
#include<SFML/Graphics.hpp>

sf::Vector2f find_velocity_at_point(sf::Vector2f phys_position, const float* hvels, const float* vvels, const Dimensions& dims);
void set_walls_dirichlet_boundary_conditions(float* hvels, float* vvels, const Dimensions& dims, const int* obstacles, const int obstacles_count);
void solve_divergences(const float* hvels, const float* vvels, const Dimensions& dims, float *divergences, const int iterations, const float overrelaxation);
void solve_pressure_for_divergence_free_velocity_field(float* hvels, float* vvels, float* pressures, const Dimensions& dims, const float ρ, const std::vector<bool>& walls, const float dt, const int iterations);
void apply_pressure_gradient_to_velocity_field(float* hvels, float* vvels, const float* pressures, const Dimensions& dims, const float ρ, const float dt);
void apply_gravity_to_velocity_field(float *vvels, const Dimensions &dims, const std::vector<bool> &walls, const float g, const float dt);
void advect_velocities(float *hvels, float *vvels, const Dimensions &dims, const std::vector<bool> &walls, const float dt, const int iterator_type);
