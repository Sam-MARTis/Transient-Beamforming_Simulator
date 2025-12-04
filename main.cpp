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
float *EfieldsX = new float[NX* NY];
float *EfieldsY = new float[NX* NY];
float *Emags = new float[NX* NY];

// GUI variables
float *rand_property = new float[NX * NY];
static int current_mode = 1;
const char *modes[] = {"Magnitude", "Field Strength", "Pressure"};
static float color1[3] = {1.0f, 0.0f, 0.0f};
static float color2[3] = {0.0f, 0.0f, 1.0f};



float DT = DT_default;

bool apply_gravity = false;
float gravity_acceleration = 9.81f;


sf::RenderWindow window;
sf::Vector2i mouse_previous_screen_pos;
float dt_inner = 0.016f;
float sim_time = 0.0f;

const float elements_fraction = 0.4f;
const int num_transmitter_elements = 5;
bool play_simulation = false;
float Ey_range = 0.1f;
float frequency_transmitter = 1.0f;
float csq = 10.0f;
float dphase_net = 1.0f;
float avg_disp_qty = 0.0f;

int speed_up_factor = 1;
int main()
{
    window.create(sf::VideoMode({SCREEN_WIDTH + (SCREEN_OFFSET_X + SCREEN_END_X_PADDING), SCREEN_HEIGHT + (SCREEN_OFFSET_Y + SCREEN_END_Y_PADDING)}, 10), "Fluid Simulation");
    window.setFramerateLimit(FRAME_RATE_LIMIT);
    ImGui::SFML::Init(window);
    initialize_shapes(main_shapes, NX, NY, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_OFFSET_X, SCREEN_OFFSET_Y);

    for(int i =0; i<NX; i++){
        for(int j=0; j<NY; j++){
            EfieldsX[FLAT(i, j, NX)] = 0.0f;
            EfieldsY[FLAT(i, j, NX)] = 0.0f;
        }
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

    sf::Clock deltaClock;
    set_walls_dirichlet_boundary_conditions(EfieldsX, EfieldsY, sim_dimensions, sim_time, dphase_net, num_transmitter_elements, elements_fraction, frequency_transmitter);
    while (window.isOpen())
    {

        if (play_simulation)
        {
            for(int i=0; i< speed_up_factor; i++){
                
                set_walls_dirichlet_boundary_conditions(EfieldsX, EfieldsY, sim_dimensions, sim_time, dphase_net, num_transmitter_elements, elements_fraction, frequency_transmitter);
                step_time(EfieldsX, EfieldsY, sim_dimensions, csq, DT);
                sim_time += DT;
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
        

        ImGui::NewLine();
        ImGui::Checkbox("Solve", &play_simulation);
        ImGui::SliderFloat("Input Frequency (Hz)", &frequency_transmitter, 0.1f, 10.0f, "%.3f Hz");
        ImGui::InputFloat("C^2", &csq, 1.0f, 100.0f, "%.3f");
        ImGui::InputInt("Speed Up Factor", &speed_up_factor, 1, 50);
        ImGui::InputFloat("Net Phase Delay (rad)", &dphase_net, 0.1f, 3.14f, "%.3f rad");
        ImGui::SliderFloat("Offset Display", &avg_disp_qty, -10.0f*Ey_range, 10.0f*Ey_range, "%.3f");
        ImGui::NewLine();

        ImGui::Text("Rendering Options");
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
        // if (current_mode == DISPLAY_DIVERGENCE_INDEX)
        {
            ImGui::Spacing();
            ImGui::SliderFloat("Electric Y Range", &Ey_range, 0.0001f , 0.005f , "%.3f");
            ImGui::Text("Electric Color Settings");
            ImGui::ColorEdit3("Color 1", color1);
            ImGui::ColorEdit3("Color 2", color2);
            ImGui::Spacing();
        }
        // else if (current_mode == DISPLAY_PRESSURE_INDEX)
        // {
        //     ImGui::Spacing();
        //     ImGui::SliderFloat("Pressure Range", &pressure_magnitude_range, 0.1f, 500.0f, "%.3f");
        //     ImGui::Text("Pressure Color Settings");
        //     ImGui::ColorEdit3("Color 1", color1);
        //     ImGui::ColorEdit3("Color 2", color2);
        //     ImGui::Spacing();
        // }

        // ImGui::Checkbox("Render Edge Velocities", &render_edge_velocities);
        // if (render_edge_velocities)
        // {

        //     ImGui::Spacing();
        //     if (ImGui::CollapsingHeader("Edge Velocity Settings"))
        //     {
        //         ImGui::SliderInt("Arrow Thickness", &arrow_thickness, 1, 5);
        //         ImGui::SliderFloat("Arrow Head Fraction", &head_fraction, 0.01f, 0.7f, "%.3f");
        //         ImGui::SliderFloat("Arrow Normalization", &arrow_normalization, 0.1f, 5.0f, "%.2f");
        //         ImGui::SliderFloat("Arrow Max Size", &arrow_max_size, 0.1f, 100.0f, "%.2f");
        //         ImGui::ColorEdit3("Arrow Color", arrow_color);
        //     }
        // }
        // ImGui::Checkbox("Render Flow Field", &render_flow_field);
        // if (render_flow_field)
        // {
        //     ImGui::Spacing();
        //     if (ImGui::CollapsingHeader("Flow Field Settings"))
        //     {
        //         ImGui::SliderFloat("Flow Field Density X", &flow_field_density_x, 0.01f, 1.0f, "%.3f");
        //         ImGui::SliderFloat("Flow Field Density Y", &flow_field_density_y, 0.01f, 1.0f, "%.3f");
        //         ImGui::SliderInt("Flow Arrow Thickness", &flow_arrow_thickness, 1, 5);
        //         ImGui::SliderFloat("Flow Arrow Head Fraction", &flow_arrow_head_fraction, 0.01f, 0.7f, "%.3f");
        //         ImGui::SliderFloat("Flow Arrow Normalization", &flow_arrow_normalization, 0.1f, 5.0f, "%.2f");
        //         ImGui::SliderFloat("Flow Arrow Max Size", &flow_arrow_max_size, 0.1f, 100.0f, "%.2f");
        //         ImGui::ColorEdit3("Flow Field Color", flow_field_color);
        //     }
        // }

        ImGui::End();

        window.clear(sf::Color::Black);
        if (play_simulation)
        {
            // sim_time += DT;
            get_field_strength_magnitude(EfieldsX, EfieldsY, sim_dimensions, Emags);

        
        
            display_shapes(window, main_shapes, sim_dimensions, Emags, avg_disp_qty, Ey_range + avg_disp_qty, convert_float_to_sf_colour(color1), convert_float_to_sf_colour(color2));
            // if (current_mode == DISPLAY_DIVERGENCE_INDEX)
            // {
            // }
            // else if (current_mode == DISPLAY_DEFAULT_INDEX)
            // {
            //     display_shapes(window, main_shapes, sim_dimensions, nullptr, 0.0f, 1.0f, sf::Color::Red, sf::Color::Blue);
            // }
            // else if (current_mode == DISPLAY_PRESSURE_INDEX)
            // {
            //     display_shapes(window, main_shapes, sim_dimensions, pressures, -pressure_magnitude_range, pressure_magnitude_range, convert_float_to_sf_colour(color1), convert_float_to_sf_colour(color2));
            // }
        }
        // if (render_edge_velocities)
        // {
        //     display_edge_velocities(window, hvels, vvels, sim_dimensions, arrow_normalization, arrow_max_size, arrow_thickness, head_fraction, convert_float_to_sf_colour(arrow_color));
        // }
        // if (render_flow_field)
        // {
        //     display_flow_field(window, hvels, vvels, sim_dimensions, flow_field_density_x, flow_field_density_y, flow_arrow_normalization, flow_arrow_max_size, flow_arrow_thickness, flow_arrow_head_fraction, convert_float_to_sf_colour(flow_field_color));
        // }

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}