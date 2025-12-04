#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
#include"constants.hpp"

void print(const auto message);
auto clamp(const auto value, const auto min, const auto max);
float clampf(const float value, const float min, const float max);
float randf(const float min, const float max);
float map_physics_to_screen_x(const float phys_coord_x, const float screen_offset_x, const float screen_physics_x_ratio);
float map_physics_to_screen_y(const float phys_coord_y, const float screen_offset_y, const float screen_physics_y_ratio);
float map_screen_to_physics_x(const float screen_coord_x, const float screen_offset_x, const float screen_physics_x_ratio);
float map_screen_to_physics_y(const float screen_coord_y, const float screen_offset_y, const float screen_physics_y_ratio);
sf::Color convert_float_to_sf_colour(float color_float[3]);
void impart_velocity_to_fluid_field(float *hvels, float *vvels, const Dimensions &dims, const float center_x, const float center_y, const float radius, const sf::Vector2f velocity);
void impart_acceleration_to_fluid_field(float *hvels, float *vvels, const Dimensions &dims, const float center_x, const float center_y, const float radius, const sf::Vector2f acceleration, const float dt);
