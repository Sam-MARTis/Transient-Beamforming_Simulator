#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "aux-functions.hpp"
#include "core-sim-functions.hpp"

void display_shapes(sf::RenderWindow &window, const sf::RectangleShape *shapes, const Dimensions &dims, const float *property_to_display, const float property_min, const float property_max, sf::Color colour_high, sf::Color colour_low)
{
    // Performance not critical here, so chill

    const int cells_x = dims.nx;
    const int cells_y = dims.ny;
    const float normalization_factor = 1.0f / (property_max - property_min);
    for (int i = 0; i < cells_x; i++)
    {
        for (int j = 0; j < cells_y; j++)
        {
            const int idx = FLAT(i, j, cells_x);
            sf::RectangleShape shape = shapes[idx];
            if (property_to_display != nullptr)
            {
                const float prop_value = property_to_display[idx];
                const float clamped_value = clampf(prop_value, property_min, property_max);
                const float normalized_value = (clamped_value - property_min) * normalization_factor;
                const sf::Color color_value = sf::Color(
                    (colour_low.r + normalized_value * (colour_high.r - colour_low.r)),
                    (colour_low.g + normalized_value * (colour_high.g - colour_low.g)),
                    (colour_low.b + normalized_value * (colour_high.b - colour_low.b)),
                    200);
                shape.setFillColor(color_value);
            }
            else
            {
                shape.setFillColor(CELL_FILL_COLOUR_default);
            }
            window.draw(shape);
        }
    }
}

