
#include "constants.hpp"
#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

// Just 0 boundary condition sufficienct for edges for now
#define HVELS(i, j, nx, ny) ((i >= 0) && (i <= (nx - 1)) && (j >= 0) && (j <= (ny - 1))) ? hvels[FLAT(i, j, nx + 1)] : 0.0f
#define VVELS(i, j, nx, ny) ((i >= 0) && (i <= (nx - 1)) && (j >= 0) && (j <= (ny - 1))) ? vvels[FLAT(i, j, nx)] : 0.0f

#define INTERPOLATE(a, b, frac) (((1.0f - ((float)(frac))) * ((float)a) + ((float)(frac)) * ((float)b)))

sf::Vector2f find_velocity_at_point(sf::Vector2f phys_position, const float *hvels, const float *vvels, const Dimensions &dims)
{
    const int nx = dims.nx;
    const int ny = dims.ny;
    const float cell_x = (float)dims.size_physics_x_max / (float)dims.nx;
    const float cell_y = (float)dims.size_physics_y_max / (float)dims.ny;
    const float x = phys_position.x / cell_x;
    const float y = phys_position.y / cell_y;
    if ((x < 0) || (x >= nx) || (y < 0) || (y >= ny))
    {
        return sf::Vector2f(0.0f, 0.0f);
    }
    const int i = floor(x);
    const int j = floor(y);
    const float i_frac = x - i;
    const float j_frac = y - j;

    const bool left = (i_frac < 0.5f);
    const bool top = (j_frac < 0.5f);
    float hvel_1, hvel_2, hvel_3, hvel_4;
    float vvel_1, vvel_2, vvel_3, vvel_4;
    float hvel_top, hvel_bottom;
    float vvel_left, vvel_right;
    float hvel = 0.0f;
    float vvel = 0.0f;
    switch (2 * (int)left + 1 * (int)top)
    {
    case 0: // bottom right
        hvel_1 = HVELS(i, j, nx, ny);
        hvel_2 = HVELS(i + 1, j, nx, ny);
        hvel_3 = HVELS(i, j + 1, nx, ny);
        hvel_4 = HVELS(i + 1, j + 1, nx, ny);
        hvel_top = INTERPOLATE(hvel_1, hvel_2, i_frac);
        hvel_bottom = INTERPOLATE(hvel_3, hvel_4, i_frac);
        hvel = INTERPOLATE(hvel_top, hvel_bottom, j_frac - 0.5f);

        vvel_1 = VVELS(i, j, nx, ny);
        vvel_2 = VVELS(i + 1, j, nx, ny);
        vvel_3 = VVELS(i, j + 1, nx, ny);
        vvel_4 = VVELS(i + 1, j + 1, nx, ny);
        vvel_left = INTERPOLATE(vvel_1, vvel_3, j_frac);
        vvel_right = INTERPOLATE(vvel_2, vvel_4, j_frac);
        vvel = INTERPOLATE(vvel_left, vvel_right, i_frac - 0.5f);

        // Verified

        return sf::Vector2f(hvel, vvel);
    case 1: // top right
        hvel_1 = HVELS(i, j - 1, nx, ny);
        hvel_2 = HVELS(i + 1, j - 1, nx, ny);
        hvel_3 = HVELS(i, j, nx, ny);
        hvel_4 = HVELS(i + 1, j, nx, ny);

        hvel_top = INTERPOLATE(hvel_1, hvel_2, i_frac);
        hvel_bottom = INTERPOLATE(hvel_3, hvel_4, i_frac);
        hvel = INTERPOLATE(hvel_top, hvel_bottom, j_frac + 0.5f);

        vvel_1 = VVELS(i, j, nx, ny);
        vvel_2 = VVELS(i + 1, j, nx, ny);
        vvel_3 = VVELS(i, j + 1, nx, ny);
        vvel_4 = VVELS(i + 1, j + 1, nx, ny);
        vvel_left = INTERPOLATE(vvel_1, vvel_3, j_frac);
        vvel_right = INTERPOLATE(vvel_2, vvel_4, j_frac);
        vvel = INTERPOLATE(vvel_left, vvel_right, i_frac - 0.5f);
        return sf::Vector2f(hvel, vvel);

        // Verified

    case 2: // bottom left
        hvel_1 = HVELS(i, j, nx, ny);
        hvel_2 = HVELS(i + 1, j, nx, ny);
        hvel_3 = HVELS(i, j + 1, nx, ny);
        hvel_4 = HVELS(i + 1, j + 1, nx, ny);
        hvel_top = INTERPOLATE(hvel_1, hvel_2, i_frac);
        hvel_bottom = INTERPOLATE(hvel_3, hvel_4, i_frac);
        hvel = INTERPOLATE(hvel_top, hvel_bottom, j_frac - 0.5f);

        vvel_1 = VVELS(i - 1, j, nx, ny);
        vvel_2 = VVELS(i, j, nx, ny);
        vvel_3 = VVELS(i - 1, j + 1, nx, ny);
        vvel_4 = VVELS(i, j + 1, nx, ny);
        vvel_left = INTERPOLATE(vvel_1, vvel_3, j_frac);
        vvel_right = INTERPOLATE(vvel_2, vvel_4, j_frac);
        vvel = INTERPOLATE(vvel_left, vvel_right, i_frac + 0.5f);
        return sf::Vector2f(hvel, vvel);

        // Verified

    case 3: // top left
        hvel_1 = HVELS(i, j - 1, nx, ny);
        hvel_2 = HVELS(i + 1, j - 1, nx, ny);
        hvel_3 = HVELS(i, j, nx, ny);
        hvel_4 = HVELS(i + 1, j, nx, ny);
        hvel_top = INTERPOLATE(hvel_1, hvel_2, i_frac);
        hvel_bottom = INTERPOLATE(hvel_3, hvel_4, i_frac);
        hvel = INTERPOLATE(hvel_top, hvel_bottom, j_frac + 0.5f);

        vvel_1 = VVELS(i - 1, j, nx, ny);
        vvel_2 = VVELS(i, j, nx, ny);
        vvel_3 = VVELS(i - 1, j + 1, nx, ny);
        vvel_4 = VVELS(i, j + 1, nx, ny);
        vvel_left = INTERPOLATE(vvel_1, vvel_3, j_frac);
        vvel_right = INTERPOLATE(vvel_2, vvel_4, j_frac);
        vvel = INTERPOLATE(vvel_left, vvel_right, i_frac + 0.5f);
        return sf::Vector2f(hvel, vvel);
    }
}

void set_walls_dirichlet_boundary_conditions(float *hvels, float *vvels, const Dimensions &dims, const int *obstacles, const int obstacles_count)
{
    const int nx = dims.nx;
    const int ny = dims.ny;
    for (int j = 0; j < ny; j++)
    {
        hvels[FLAT(0, j, nx + 1)] = 0.0f;
        hvels[FLAT(nx, j, nx + 1)] = 0.0f;
    }
    for (int i = 0; i < nx; i++)
    {
        vvels[FLAT(i, 0, nx)] = 0.0f;
        vvels[FLAT(i, ny, nx)] = 0.0f;
    }
    for (int obs_i = 0; obs_i < obstacles_count; obs_i++)
    {
        const int obs_idx = obstacles[obs_i];
        const int obs_x = obs_idx % nx;
        const int obs_y = obs_idx / nx;
        hvels[FLAT(obs_x, obs_y, nx + 1)] = 0.0f;
        hvels[FLAT(obs_x + 1, obs_y, nx + 1)] = 0.0f;
        vvels[FLAT(obs_x, obs_y, nx)] = 0.0f;
        vvels[FLAT(obs_x, obs_y + 1, nx)] = 0.0f;
    }
}
void set_walls_no_slip_boundary_conditions(float *hvels, float *vvels, const Dimensions &dims, const int *obstacles, const int obstacles_count)
{
    // To be implemented later
}

void solve_divergences(float *hvels, float *vvels, const Dimensions &dims, float *divergences, const int iterations, const float overrelaxation)
{
  

    const int nx = dims.nx;
    const int ny = dims.ny;
    for(int iter = 0; iter< iterations; iter ++){
        for (int j = 0; j < ny; j++)
        {
            for (int i = 0; i < nx; i++)
            {
                const float hvel_right = HVELS(i + 1, j, nx, ny);
                const float hvel_left = HVELS(i, j, nx, ny);
                const float vvel_top = VVELS(i, j + 1, nx, ny);
                const float vvel_bottom = VVELS(i, j, nx, ny);
                float& right = hvels[FLAT(i+1, j, nx+1)];
                float& left = hvels[FLAT(i, j, nx+1)];
                float& top = vvels[FLAT(i, j, nx)];
                float& bottom = vvels[FLAT(i, j+1, nx)];
                
                const float divergence_addition  = ((right-left) + (top - bottom))/4;
                
                right -= divergence_addition;
                left += divergence_addition;
                top -= divergence_addition;
                bottom += divergence_addition;


            }
        }
    }


}

#define PRESSURES(i, j, nx, ny) (((i) >= 0) && ((i) < (nx)) && ((j) >= 0) && ((j) < (ny))) ? pressures[FLAT(i, j, nx)] : 0.0f
inline bool WALLS(int i, int j, const int &nx, const int &ny, const std::vector<bool> &walls) { return (((i) >= 0) && ((i) < (nx)) && ((j) >= 0) && ((j) < (ny))) ? walls[FLAT(i, j, nx)] : true; }
void solve_pressure_for_divergence_free_velocity_field(float *hvels, float *vvels, float *pressures, const Dimensions &dims, const float ρ, const std::vector<bool> &walls, const float dt, const int iterations)
{
    const int nx = dims.nx;
    const int ny = dims.ny;
    const float dx = (float)dims.size_physics_x_max / (float)dims.nx;
    const float dy = (float)dims.size_physics_y_max / (float)dims.ny;
    const float dy_by_dx = dy / dx;
    const float dx_by_dy = dx / dy;
    const float dx_by_dt = dx / dt;
    const float dy_by_dt = dy / dt;
    const float inv_dx2 = 1.0f / (dx * dx);
    for (int iter = 0; iter < iterations; iter++)
    {
        // It is assumed that all boundary cells are walls
        for (int i = 0; i < nx; i++)
        {
            for (int j = 0; j < ny; j++)
            {
                const int idx = FLAT(i, j, nx);
                if (walls[idx] == false)
                {
                    const char num_fluid_left_right = ((char)1 - (char)WALLS(i - 1, j, nx, ny, walls)) + ((char)1 - (char)WALLS(i + 1, j, nx, ny, walls));
                    const char num_fluid_top_bottom = ((char)1 - (char)WALLS(i, j - 1, nx, ny, walls)) + ((char)1 - (char)WALLS(i, j + 1, nx, ny, walls));
                    const float den = dy_by_dx * (float)num_fluid_left_right + dx_by_dy * (float)num_fluid_top_bottom;
                    const float Pr = PRESSURES(i + 1, j, nx, ny);
                    const float Pl = PRESSURES(i - 1, j, nx, ny);
                    const float Pu = PRESSURES(i, j - 1, nx, ny);
                    const float Pd = PRESSURES(i, j + 1, nx, ny);
                    const float hvel_r = HVELS(i + 1, j, nx, ny);
                    const float hvel_l = HVELS(i, j, nx, ny);
                    const float vvel_u = VVELS(i, j, nx, ny);
                    const float vvel_d = VVELS(i, j + 1, nx, ny);
                    const float rhs = (Pr + Pl) * dy_by_dx + (Pd + Pu) * dx_by_dy + ρ * ((vvel_u - vvel_d) * dx_by_dt + (hvel_l - hvel_r) * dy_by_dt);
                    pressures[idx] = rhs / den;
                }
                else
                {
                    pressures[idx] = 0.0f;
                    continue;
                }
            }
        }
    }
}

void apply_pressure_gradient_to_velocity_field(float *hvels, float *vvels, const float *pressures, const Dimensions &dims, const float ρ, const float dt)
{
    // Remember to apply boundary conditions after this
    const int nx = dims.nx;
    const int ny = dims.ny;
    const float dx = (float)dims.size_physics_x_max / (float)dims.nx;
    const float dy = (float)dims.size_physics_y_max / (float)dims.ny;
    const float inv_ρ = 1.0f / ρ;
    const float kx = dt / (ρ * dx);
    const float ky = dt / (ρ * dy);
    for (int j = 0; j < ny; j++)
    {
        for (int i = 0; i < nx; i++)
        {
            // const int idx =
            const float P = PRESSURES(i, j, nx, ny);
            hvels[FLAT(i, j, nx + 1)] -= kx * P;
            hvels[FLAT(i + 1, j, nx + 1)] += kx * P;
            vvels[FLAT(i, j, nx)] -= ky * P;
            vvels[FLAT(i, j + 1, nx)] += ky * P;
        }
    }
}

void advect_velocities(float *hvels, float *vvels, const Dimensions &dims, const std::vector<bool> &walls, const float dt, const int iterator_type)
{
    float *temp_hvels = new float[(dims.nx + 1) * dims.ny];
    float *temp_vvels = new float[dims.nx * (dims.ny + 1)];
    // Implement advection here
    const int nx = dims.nx;
    const int ny = dims.ny;
    const float cell_x = (float)dims.size_physics_x_max / (float)dims.nx;
    const float cell_y = (float)dims.size_physics_y_max / (float)dims.ny;
    const float one_sixths = 1.0f / 6.0f;
    // Advect horizontal velocities first
    for (int i = 1; i < (nx); i++)
    {
        for (int j = 0; j < (ny); j++)
        {
            if (!walls[FLAT(i, j, nx)])
            {
                sf::Vector2f p1 = sf::Vector2f((float)i * cell_x, ((float)j + 0.5f) * cell_y);
                if (iterator_type == RK4_INDEX)
                { // RK4
                    sf::Vector2f k1 = find_velocity_at_point(p1, hvels, vvels, dims);
                    sf::Vector2f p2 = p1 - 0.5f * dt * k1;
                    sf::Vector2f k2 = find_velocity_at_point(p2, hvels, vvels, dims);
                    sf::Vector2f p3 = p1 - 0.5f * dt * k2;
                    sf::Vector2f k3 = find_velocity_at_point(p3, hvels, vvels, dims);
                    sf::Vector2f p4 = p1 - dt * k3;
                    sf::Vector2f k4 = find_velocity_at_point(p4, hvels, vvels, dims);
                    sf::Vector2f vel = (one_sixths * (k1 + 2.0f * k2 + 2.0f * k3 + k4));
                    temp_hvels[FLAT(i, j, nx + 1)] = vel.x;
                }
                else if (iterator_type == RK2_INDEX)
                { // RK2
                    sf::Vector2f k1 = find_velocity_at_point(p1, hvels, vvels, dims);
                    sf::Vector2f p2 = p1 - 0.5f * dt * k1;
                    sf::Vector2f k2 = find_velocity_at_point(p2, hvels, vvels, dims);
                    sf::Vector2f vel = k2;
                    temp_hvels[FLAT(i, j, nx + 1)] = vel.x;
                }
            }
        }
    }
    // Advect vertical velocities next
    for (int i = 0; i < (nx); i++)
    {
        for (int j = 1; j < (ny); j++)
        {
            if (!walls[FLAT(i, j, nx)])
            {
                sf::Vector2f p1 = sf::Vector2f(((float)i + 0.5f) * cell_x, (float)j * cell_y);
                if (iterator_type == RK4_INDEX)
                { // RK4
                    sf::Vector2f k1 = find_velocity_at_point(p1, hvels, vvels, dims);
                    sf::Vector2f p2 = p1 - 0.5f * dt * k1;
                    sf::Vector2f k2 = find_velocity_at_point(p2, hvels, vvels, dims);
                    sf::Vector2f p3 = p1 - 0.5f * dt * k2;
                    sf::Vector2f k3 = find_velocity_at_point(p3, hvels, vvels, dims);
                    sf::Vector2f p4 = p1 - dt * k3;
                    sf::Vector2f k4 = find_velocity_at_point(p4, hvels, vvels, dims);
                    sf::Vector2f vel = (one_sixths * (k1 + 2.0f * k2 + 2.0f * k3 + k4));
                    temp_vvels[FLAT(i, j, nx)] = vel.y;
                }
                else if (iterator_type == RK2_INDEX)
                { // RK2
                    sf::Vector2f k1 = find_velocity_at_point(p1, hvels, vvels, dims);
                    sf::Vector2f p2 = p1 - 0.5f * dt * k1;
                    sf::Vector2f k2 = find_velocity_at_point(p2, hvels, vvels, dims);
                    sf::Vector2f vel = k2;
                    temp_vvels[FLAT(i, j, nx)] = vel.y;
                }
            }
        }
    }     

    // Copy temp arrays to original arrays at the end
    for (int i=0; i< (nx +1)* ny; i++){
        hvels[i] = temp_hvels[i];
    }
    for (int i=0; i< nx * (ny +1); i++){
        vvels[i] = temp_vvels[i];
    }
    
    delete[] temp_hvels;
    delete[] temp_vvels;
}

void apply_gravity_to_velocity_field(float *vvels, const Dimensions &dims, const std::vector<bool> &walls, const float g, const float dt)
{
    const int nx = dims.nx;
    const int ny = dims.ny;
    const float g_dt = g * dt;
    for (int i = 0; i < nx; i++)
    {
        for (int j = 1; j < ny ; j++)
        {
            const int idx = FLAT(i, j, nx);
            if (!walls[idx])
            {
                vvels[idx] += g_dt;
            }
        }
    }
}