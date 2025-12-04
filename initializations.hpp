#pragma once
#include <SFML/Graphics.hpp>

void initialize_velocities(float* hvels, float* vvels, const int cells_x, const int cells_y, const float vx, const float vy);
void initialize_shapes(sf::RectangleShape* shapes, const int cells_x, const int  cells_y, const int screen_width, const int screen_height, const int screen_offset_x, const int screen_offset_y);

