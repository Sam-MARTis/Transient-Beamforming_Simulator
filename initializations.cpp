#include <SFML/Graphics.hpp>
#include "constants.hpp"

void initialize_velocities(float* hvels, float* vvels, const int cells_x, const int cells_y, const float vx, const float vy){
    for(int i = 0; i < (cells_x+1) * cells_y; ++i) {
        hvels[i] = vx;
    }
    for(int i = 0; i < cells_x * (cells_y+1); ++i) {
        vvels[i] = vy;
    }
};
void initialize_shapes(sf::RectangleShape* shapes, const int cells_x, const int cells_y, const int screen_width, const int screen_height, const int screen_offset_x, const int screen_offset_y){
    const float cell_dx = (float)screen_width / cells_x;
    const float cell_dy = (float)screen_height / cells_y;
    for(int i=0; i<cells_x; i++){
        for(int j=0; j<cells_y; j++){
            sf::RectangleShape& shape = shapes[FLAT(i, j, cells_x)];
            shape.setSize(sf::Vector2f(cell_dx, cell_dy));
            shape.setPosition({screen_offset_x + i * cell_dx, screen_offset_y + j * cell_dy});
            shape.setFillColor(CELL_FILL_COLOUR_default);
            shape.setOutlineColor(CELL_OUTLINE_COLOUR_default);
            shape.setOutlineThickness(CELL_OUTLINE_THICKNESS_default);
        }
    }
};

