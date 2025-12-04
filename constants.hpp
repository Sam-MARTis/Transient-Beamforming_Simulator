#pragma once


#define SQ(x) ((x) * (x))
#define NORM2SQ(x, y) (SQ(x) + SQ(y))
#define NORM2(x, y) (sqrt(NORM2SQ(x, y)))
#define FLAT(x, y, width) ((y) * (width) + (x))


#define SIZE_PHYSICS_X_MAX_default 16
#define SIZE_PHYSICS_Y_MAX_default 9 
#define DENSITY_CELLS 10
#define NX_default SIZE_PHYSICS_X_MAX_default * DENSITY_CELLS
#define NY_default SIZE_PHYSICS_Y_MAX_default * DENSITY_CELLS
// KEEP IT A SQUARE GRID. In the divergence free pressure solver, I assume square cells. Fix that before changing this.

#define INITIAL_X_VELOCITY_default 0.0f
#define INITIAL_Y_VELOCITY_default 0.0f
#define DIVERGENCE_ITERATIONS_DEFAULT 5
#define DT_default 0.1f
#define MIN_DT 0.0001f
// Cosmetic
#define SCREEN_OFFSET_X_default 100
#define SCREEN_OFFSET_Y_default 150
#define SCREEN_END_X_PADDING_default 50
#define SCREEN_END_Y_PADDING_default 250
#define SCREEN_WIDTH_default 1000
#define SCREEN_HEIGHT_default 600
#define CELL_OUTLINE_THICKNESS_default 1.0f
#define CELL_OUTLINE_COLOUR_default sf::Color(100, 100, 100, 255)
#define CELL_FILL_COLOUR_default sf::Color(255, 255, 255, 100)

// Other
// Options
#define DISPLAY_DEFAULT_INDEX 0
#define DISPLAY_DIVERGENCE_INDEX 1
#define DISPLAY_PRESSURE_INDEX 2


#define RK2_INDEX 0
#define RK4_INDEX 1




// // Derived quantities from above
// #define CELL_PHYSICS_DX ((float)SIZE_PHYSICS_X_MAX / NX)
// #define CELL_PHYSICS_DY ((float)SIZE_PHYSICS_Y_MAX / NY)
// #define CELL_SCREEN_DX ((float)SCREEN_WIDTH / NX)
// #define CELL_SCREEN_DY ((float)SCREEN_HEIGHT / NY)


// Renderer parameters
#define FRAME_RATE_LIMIT 60


struct Dimensions{
    int nx;
    int ny;
    int size_physics_x_max;
    int size_physics_y_max;
    int screen_width;
    int screen_height;
    int screen_offset_x;
    int screen_offset_y;
};



// #define DT 0.01f
// #define MAX_DIVERGENCE_ITERATIONS 10
// #define DIVERGENCE_TOLERANCE 0.001f
// #define MAX_ADVECTION_SHORTENING_ITERATIONS 100
// The above should all be variable from the interface