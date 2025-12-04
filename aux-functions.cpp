#include <random>
#include "aux-functions.hpp" 
#include "constants.hpp"
#include<SFML/Graphics.hpp>
#include<math.h>


// const float PHYSICS_SCREEN_X_RATIO = SIZE_PHYSICS_X_MAX / SCREEN_WIDTH;
// const float PHYSICS_SCREEN_Y_RATIO = SIZE_PHYSICS_Y_MAX / SCREEN_HEIGHT;
// const float SCREEN_PHYSICS_X_RATIO = SCREEN_WIDTH / SIZE_PHYSICS_X_MAX;
// const float SCREEN_PHYSICS_Y_RATIO = SCREEN_HEIGHT / SIZE_PHYSICS_Y_MAX;


void print(const auto message)
{
    std::cout << message << std::endl;
};
auto clamp(const auto value, const auto min, const auto max)
{
    return (value < min) ? min : (value > max) ? max : value;
}
float clampf(const float value, const float min, const float max){
    return (value < min) ? min : (value > max) ? max : value;
};
float randf(const float min, const float max){
    return min + ((float)rand()/RAND_MAX) * (max - min);
};

float map_physics_to_screen_x(const float phys_coord_x, const float screen_offset_x, const float screen_physics_x_ratio){
    return screen_offset_x + (phys_coord_x * screen_physics_x_ratio);
}
float map_physics_to_screen_y(const float phys_coord_y, const float screen_offset_y, const float screen_physics_y_ratio){
    return screen_offset_y + (phys_coord_y * screen_physics_y_ratio);
}
float map_screen_to_physics_x(const float screen_coord_x, const float screen_offset_x, const float screen_physics_x_ratio){
    return ((screen_coord_x - screen_offset_x) * screen_physics_x_ratio);
}
float map_screen_to_physics_y(const float screen_coord_y, const float screen_offset_y, const float screen_physics_y_ratio){
    return ((screen_coord_y - screen_offset_y) * screen_physics_y_ratio);
}

sf::Color convert_float_to_sf_colour(float color_float[3])
{
    return sf::Color(
        clampf(color_float[0], 0.0f, 1.0f) * 255,
        clampf(color_float[1], 0.0f, 1.0f) * 255,
        clampf(color_float[2], 0.0f, 1.0f) * 255
    );
}

void impart_velocity_to_fluid_field(float *hvels, float *vvels, const Dimensions &dims, const float center_x, const float center_y, const float radius, const sf::Vector2f velocity){
    if(center_x < 0 || center_x >= dims.size_physics_x_max || center_y < 0 || center_y >= dims.size_physics_y_max){
        return;
    }
    const int nx = dims.nx;
    const int ny = dims.ny;
    const float cell_x = (float)dims.size_physics_x_max / (float)dims.nx;
    const float cell_y = (float)dims.size_physics_y_max / (float)dims.ny;
    const int center_idx_x = ceil(center_x / cell_x);
    const int center_idx_y = ceil(center_y / cell_y);
    const int lower_x_idx = clamp(floor((center_x - radius) / cell_x), 0, nx);
    const int upper_x_idx = clamp(floor((center_x + radius) / cell_x), 0, nx);
    const int lower_y_idx = clamp(floor((center_y - radius) / cell_y), 0, ny);
    const int upper_y_idx = clamp(floor((center_y + radius) / cell_y), 0, ny);

    for(int j = lower_y_idx; j <= upper_y_idx; j++){
        for(int i = lower_x_idx; i <= upper_x_idx; i++){
            hvels[FLAT(i, j, nx + 1)] = velocity.x;
            hvels[FLAT(i+1, j, nx + 1)] = velocity.x;
            hvels[FLAT(i, j+1, nx + 1)] = velocity.x;
            hvels[FLAT(i+1, j+1, nx + 1)] = velocity.x;
            vvels[FLAT(i, j, nx)] = velocity.y;
            vvels[FLAT(i, j+1, nx)] = velocity.y;
            vvels[FLAT(i+1, j, nx)] = velocity.y;
            vvels[FLAT(i+1, j+1, nx)] = velocity.y;
            
        }
    }


}