#pragma once
#include <SFML/Graphics.hpp>
#include "constants.hpp"

float display_shapes(sf::RenderWindow &window, const sf::RectangleShape *shapes, const Dimensions &dims, const float *property_to_display, const float property_min, const float property_max, sf::Color colour_high, sf::Color colour_low);