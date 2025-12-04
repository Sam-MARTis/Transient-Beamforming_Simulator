#pragma once
#include <SFML/Graphics.hpp>
#include "constants.hpp"

float display_shapes(sf::RenderWindow &window, const sf::RectangleShape *shapes, const Dimensions &dims, const float *property_to_display, const float property_min, const float property_max, sf::Color colour_high, sf::Color colour_low);
void display_edge_velocities(sf::RenderWindow &window, const float *hvels, const float *vvels, Dimensions &dims, float normalization, float arrow_max_size, int arrow_thickness, float head_fraction, sf::Color arrow_color);
void drawArrow(sf::RenderWindow &window, sf::Vector2f start, sf::Vector2f end, int thickness = 2, float head_fraction = 0.2f, sf::Color colour = sf::Color::Red);
void display_flow_field(sf::RenderWindow &window, const float *hvels, const float *vvels, Dimensions &dims, float density_x, float density_y, float normalization, float arrow_max_size, int arrow_thickness, float head_fraction, sf::Color arrow_color);