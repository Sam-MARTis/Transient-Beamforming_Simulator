#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include "aux-functions.hpp"
#include "constants.hpp"
#include "display-functions.hpp"
#include "initializations.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "core-sim-functions.hpp"

// FYI, if some quantity is in all caps, it is not supposed to be changed within any function except the settings gui

// Defining of other global vars
int NX = NX_default;
int NY = NY_default;
int SCREEN_WIDTH = SCREEN_WIDTH_default;
int SCREEN_HEIGHT = SCREEN_HEIGHT_default;
int SCREEN_OFFSET_X = SCREEN_OFFSET_X_default;
int SCREEN_OFFSET_Y = SCREEN_OFFSET_Y_default;
int SCREEN_END_X_PADDING = SCREEN_END_X_PADDING_default;
int SCREEN_END_Y_PADDING = SCREEN_END_Y_PADDING_default;

Dimensions sim_dimensions = {
    NX,
    NY,
    SIZE_PHYSICS_X_MAX_default,
    SIZE_PHYSICS_Y_MAX_default,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    SCREEN_OFFSET_X,
    SCREEN_OFFSET_Y};

// Create the heap objects here
sf::RectangleShape *main_shapes = new sf::RectangleShape[NX * NY];
/*
Note to self: Might ditch the sfml rectangular shape object when we move to the GPU.
Might consider direct OpenGL rendering.
*/
float *hvels = new float[(NX + 1) * NY];
float *vvels = new float[NX * (NY + 1)];
float *divergences = new float[NX * NY];
float *pressures = new float[NX * NY];
std::vector<bool> walls((NX) * (NY), false);

// GUI variables
bool render_shapes = true;
float *rand_property = new float[NX * NY];
static int current_mode = 1;
const char *modes[] = {"default", "Divergence", "Pressure"};
static float color1[3] = {1.0f, 0.0f, 0.0f};
static float color2[3] = {0.0f, 0.0f, 1.0f};

bool render_edge_velocities = true;
int arrow_thickness = 2;
float head_fraction = 0.3f;
float arrow_normalization = 2.0f;
float arrow_max_size = 20.0f;
static float arrow_color[3] = {0.1f, 0.8f, 0.1f};

bool render_flow_field = true;
float flow_field_density_x = 0.1f;
float flow_field_density_y = 0.1f;
static float flow_field_color[3] = {0.8f, 0.1f, 0.8f};
float flow_arrow_normalization = 3.0f;
float flow_arrow_max_size = 20.0f;
int flow_arrow_thickness = 1;
float flow_arrow_head_fraction = 0.2f;

float divergence_magnitude_range = 2.0f;
float pressure_magnitude_range = 100.0f;

int DIVERGENCE_ITERATIONS = DIVERGENCE_ITERATIONS_DEFAULT;
float DT = DT_default;
float fluid_density = 1.225f;
bool solve_pressure_divergence_free = false;

bool apply_gravity = false;
float gravity_acceleration = 9.81f;

int current_iterator = RK4_INDEX;
const char *iterators[] = {"RK2", "RK4"};
bool advect_velocity_field = false;
sf::RenderWindow window;
sf::Vector2i mouse_previous_screen_pos;
bool is_mouse_dragging = false;
float dt_inner = 0.016f;

int main()
{
    window.create(sf::VideoMode({SCREEN_WIDTH + (SCREEN_OFFSET_X + SCREEN_END_X_PADDING), SCREEN_HEIGHT + (SCREEN_OFFSET_Y + SCREEN_END_Y_PADDING)}, 10), "Fluid Simulation");
    window.setFramerateLimit(FRAME_RATE_LIMIT);
    ImGui::SFML::Init(window);
    initialize_shapes(main_shapes, NX, NY, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_OFFSET_X, SCREEN_OFFSET_Y);

    for (int i = 0; i < (NX + 1) * NY; i++)
    {
        hvels[i] = -randf(-1.0f, 1.0f);
    }

    for (int i = 0; i < NX * (NY + 1); i++)
    {
        vvels[i] = randf(-1.0f, 1.0f);
    }

    for (int i = 0; i < NX * NY; i++)
    {
        rand_property[i] = randf(0.0f, 1.0f);
    }
    for (int i = 0; i < NX * NY; i++)
    {
        pressures[i] = 0.0f;
    }
    // for(int i=0; i< NX; i++){
    //     obstacles[FLAT(i, 0, NX)] = true;
    //     obstacles[FLAT(i, NY-1, NX)] = true;
    // }
    // for(int j=0; j< NY; j++){
    //     obstacles[FLAT(0, j, NX)] = true;
    //     obstacles[FLAT(NX-1, j, NX)] = true;
    // }

    // Okay, render loop. We got this
    float mouse_x_physics = SIZE_PHYSICS_X_MAX_default / 2.0f;
    float mouse_y_physics = SIZE_PHYSICS_Y_MAX_default / 2.0f;
    sf::Vector2i mouse_position_screen;

    sf::Clock deltaClock;
    set_walls_dirichlet_boundary_conditions(hvels, vvels, sim_dimensions, nullptr, 0);
    calculate_divergences(hvels, vvels, sim_dimensions, divergences);
    while (window.isOpen())
    {
        if (solve_pressure_divergence_free)
        {
            if(apply_gravity) {
                apply_gravity_to_velocity_field(vvels, sim_dimensions, walls, gravity_acceleration, DT);
                // set_walls_dirichlet_boundary_conditions(hvels, vvels, sim_dimensions, nullptr, 0);
                
            }
            set_walls_dirichlet_boundary_conditions(hvels, vvels, sim_dimensions, nullptr, 0);
            calculate_divergences(hvels, vvels, sim_dimensions, divergences);
            solve_pressure_for_divergence_free_velocity_field(hvels, vvels, pressures, sim_dimensions, fluid_density, walls, DT, DIVERGENCE_ITERATIONS);
            apply_pressure_gradient_to_velocity_field(hvels, vvels, pressures, sim_dimensions, fluid_density, DT);
            set_walls_dirichlet_boundary_conditions(hvels, vvels, sim_dimensions, nullptr, 0);
            calculate_divergences(hvels, vvels, sim_dimensions, divergences);

            if(advect_velocity_field) {
                advect_velocities(hvels, vvels, sim_dimensions, walls, DT, current_iterator); 
                set_walls_dirichlet_boundary_conditions(hvels, vvels, sim_dimensions, nullptr, 0);               
            }
        }
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                {
                    window.close();
                }
            }
            else if (event->is<sf::Event::MouseButtonPressed>())
            {
                mouse_previous_screen_pos = sf::Mouse::getPosition(window);
                is_mouse_dragging = true;
            }
            else if (event->is<sf::Event::MouseButtonReleased>())
            {
                is_mouse_dragging = false;
            }
            else if (event->is<sf::Event::MouseMoved>())
            {
                if (is_mouse_dragging)
                {
                    sf::Vector2i mouse_current_screen_pos = sf::Mouse::getPosition(window);
                    sf::Vector2i mouse_delta_screen_pos = mouse_current_screen_pos - mouse_previous_screen_pos;
                    sf::Vector2f mouse_current_physics_pos = sf::Vector2f(
                        (float)((mouse_current_screen_pos.x - SCREEN_OFFSET_X) * ((float)SIZE_PHYSICS_X_MAX_default / (float)SCREEN_WIDTH)),
                        (float)((mouse_current_screen_pos.y - SCREEN_OFFSET_Y) * ((float)SIZE_PHYSICS_Y_MAX_default / (float)SCREEN_HEIGHT)));
                        const float slowdown = 0.1;
                    sf::Vector2f mouse_velocity_physics = sf::Vector2f(slowdown*(float)mouse_delta_screen_pos.x * ((float)SIZE_PHYSICS_X_MAX_default / (float)SCREEN_WIDTH), slowdown*(float)mouse_delta_screen_pos.y * ((float)SIZE_PHYSICS_Y_MAX_default / (float)SCREEN_HEIGHT)) / dt_inner;

                    impart_velocity_to_fluid_field(hvels, vvels, sim_dimensions, mouse_current_physics_pos.x, mouse_current_physics_pos.y, 0.01f, mouse_velocity_physics);
                    // int mouse_x_cell = (int)((float)(mouse_current_screen_pos.x - SCREEN_OFFSET_X) * ((float)SIZE_PHYSICS_X_MAX_default / (float)SCREEN_WIDTH));
                    // int mouse_y_cell = (int)((float)(mouse_current_screen_pos.y - SCREEN_OFFSET_Y) * ((float)SIZE_PHYSICS_Y_MAX_default / (float)SCREEN_HEIGHT));
                    // const int lower_idx = clamp(mouse_x_cell, 0, NX - 1) + clamp(mouse_y_cell, 0, NY - 1) * NX;
                    // // Handle mouse dragging
                    // mouse_previous_screen_pos = mouse_current_screen_pos;
                }
            }
            else if (event->is<sf::Event::MouseWheelScrolled>())
            {
                // mouse_position_screen = sf::Mouse::getPosition(window);
                // mouse_x_physics = (float)(mouse_position_screen.x - SCREEN_OFFSET_X) * ((float)SIZE_PHYSICS_X_MAX_default / (float)SCREEN_WIDTH);
                // mouse_y_physics = (float)(mouse_position_screen.y - SCREEN_OFFSET_Y) * ((float)SIZE_PHYSICS_Y_MAX_default / (float)SCREEN_HEIGHT);
 
            }
        }
        dt_inner = deltaClock.restart().asSeconds();
        ImGui::SFML::Update(window, sf::seconds(dt_inner));
        ImGui::Begin("Settings");
        ImGui::Spacing();
        ImGui::Text("Main Params");
        ImGui::InputFloat("dt", &DT, DT_default * 0.1f, DT_default * 2.0f, "%.5f");
        DT = (DT < MIN_DT) ? MIN_DT : DT;
        ImGui::SliderFloat("Fluid Density", &fluid_density, 0.1f, 10.0f, "%.3f");

        ImGui::NewLine();
        ImGui::Text("Divergence Solver");
        ImGui::InputInt("Iter", &DIVERGENCE_ITERATIONS, 1, 10);
        ImGui::SameLine();
        ImGui::Checkbox("Solve", &solve_pressure_divergence_free);
        ImGui::NewLine();
        if (solve_pressure_divergence_free)
        {
            ImGui::Spacing();
            ImGui::Text("Gravity Settings");
            ImGui::InputFloat("g", &gravity_acceleration, 0.1f, 50.0f, "%.3f");
            ImGui::SameLine();
            ImGui::Checkbox("Apply##Gravity", &apply_gravity);

            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::Text("Advection Solver");
            if (ImGui::BeginCombo("Iterator", iterators[current_iterator]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(iterators); n++)
                {
                    bool is_selected = (current_iterator == n);
                    if (ImGui::Selectable(iterators[n], is_selected))
                    {
                        current_iterator = n;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Solve##Advection", &advect_velocity_field);
        }

        ImGui::Text("Rendering Options");
        ImGui::Checkbox("Render Shapes", &render_shapes);
        ImGui::Text("Simulation Mode");
        if (ImGui::BeginCombo("##mode_selector", modes[current_mode]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(modes); n++)
            {
                bool is_selected = (current_mode == n);
                if (ImGui::Selectable(modes[n], is_selected))
                {
                    current_mode = n;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (current_mode == DISPLAY_DIVERGENCE_INDEX)
        {
            ImGui::Spacing();
            ImGui::SliderFloat("|Div| Range", &divergence_magnitude_range, 0.05f, 5.0f, "%.3f");
            ImGui::Text("Divergence Color Settings");
            ImGui::ColorEdit3("Color 1", color1);
            ImGui::ColorEdit3("Color 2", color2);
            ImGui::Spacing();
        }
        else if (current_mode == DISPLAY_PRESSURE_INDEX)
        {
            ImGui::Spacing();
            ImGui::SliderFloat("Pressure Range", &pressure_magnitude_range, 0.1f, 500.0f, "%.3f");
            ImGui::Text("Pressure Color Settings");
            ImGui::ColorEdit3("Color 1", color1);
            ImGui::ColorEdit3("Color 2", color2);
            ImGui::Spacing();
        }

        ImGui::Checkbox("Render Edge Velocities", &render_edge_velocities);
        if (render_edge_velocities)
        {

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Edge Velocity Settings"))
            {
                ImGui::SliderInt("Arrow Thickness", &arrow_thickness, 1, 5);
                ImGui::SliderFloat("Arrow Head Fraction", &head_fraction, 0.01f, 0.7f, "%.3f");
                ImGui::SliderFloat("Arrow Normalization", &arrow_normalization, 0.1f, 5.0f, "%.2f");
                ImGui::SliderFloat("Arrow Max Size", &arrow_max_size, 0.1f, 100.0f, "%.2f");
                ImGui::ColorEdit3("Arrow Color", arrow_color);
            }
        }
        ImGui::Checkbox("Render Flow Field", &render_flow_field);
        if (render_flow_field)
        {
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Flow Field Settings"))
            {
                ImGui::SliderFloat("Flow Field Density X", &flow_field_density_x, 0.01f, 1.0f, "%.3f");
                ImGui::SliderFloat("Flow Field Density Y", &flow_field_density_y, 0.01f, 1.0f, "%.3f");
                ImGui::SliderInt("Flow Arrow Thickness", &flow_arrow_thickness, 1, 5);
                ImGui::SliderFloat("Flow Arrow Head Fraction", &flow_arrow_head_fraction, 0.01f, 0.7f, "%.3f");
                ImGui::SliderFloat("Flow Arrow Normalization", &flow_arrow_normalization, 0.1f, 5.0f, "%.2f");
                ImGui::SliderFloat("Flow Arrow Max Size", &flow_arrow_max_size, 0.1f, 100.0f, "%.2f");
                ImGui::ColorEdit3("Flow Field Color", flow_field_color);
            }
        }

        ImGui::End();

        window.clear(sf::Color::Black);
        if (render_shapes)
        {
            if (current_mode == DISPLAY_DIVERGENCE_INDEX)
            {
                display_shapes(window, main_shapes, sim_dimensions, divergences, -divergence_magnitude_range, divergence_magnitude_range, convert_float_to_sf_colour(color1), convert_float_to_sf_colour(color2));
            }
            else if (current_mode == DISPLAY_DEFAULT_INDEX)
            {
                display_shapes(window, main_shapes, sim_dimensions, nullptr, 0.0f, 1.0f, sf::Color::Red, sf::Color::Blue);
            }
            else if (current_mode == DISPLAY_PRESSURE_INDEX)
            {
                display_shapes(window, main_shapes, sim_dimensions, pressures, -pressure_magnitude_range, pressure_magnitude_range, convert_float_to_sf_colour(color1), convert_float_to_sf_colour(color2));
            }
        }
        if (render_edge_velocities)
        {
            display_edge_velocities(window, hvels, vvels, sim_dimensions, arrow_normalization, arrow_max_size, arrow_thickness, head_fraction, convert_float_to_sf_colour(arrow_color));
        }
        if (render_flow_field)
        {
            display_flow_field(window, hvels, vvels, sim_dimensions, flow_field_density_x, flow_field_density_y, flow_arrow_normalization, flow_arrow_max_size, flow_arrow_thickness, flow_arrow_head_fraction, convert_float_to_sf_colour(flow_field_color));
        }

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}