
#include "constants.hpp"
#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

// Just 0 boundary condition sufficienct for edges for now
#define HVELS(i, j, nx, ny) ((i >= 0) && (i <= (nx - 1)) && (j >= 0) && (j <= (ny - 1))) ? hvels[FLAT(i, j, nx + 1)] : 0.0f
#define VVELS(i, j, nx, ny) ((i >= 0) && (i <= (nx - 1)) && (j >= 0) && (j <= (ny - 1))) ? vvels[FLAT(i, j, nx)] : 0.0f

#define INTERPOLATE(a, b, frac) (((1.0f - ((float)(frac))) * ((float)a) + ((float)(frac)) * ((float)b)))


void set_walls_dirichlet_boundary_conditions(float *EfieldsX, float *EfieldsY, const Dimensions &dims, const float time, const float dphase_net, const int num_of_transmitter_elements, const float obstacle_fraction, const float frequency_transmitter)
{

    const int nx = dims.nx;
    const int ny = dims.ny;
    // std::cout<< "Obstacle fraction: " << obstacle_fraction << std::endl;
    // std::cout<< "Num transmitter elements: " << num_of_transmitter_elements << std::endl;
    const int obstacle_spacing = ((int)(((float)nx)*obstacle_fraction)/num_of_transmitter_elements);
    const int start_x = nx*((1-obstacle_fraction)/2);
    const float dphase_elem = dphase_net/num_of_transmitter_elements;
    // std::cout<< "Transmitter start x: " << start_x << ", spacing: " << obstacle_spacing << std::endl;
    for(int j=0; j < ny; j++){
        EfieldsX[FLAT(0, j, nx)] = EfieldsX[FLAT(1, j, nx)] ;
        EfieldsX[FLAT(nx-1, j, nx)] = EfieldsX[FLAT(nx-2, j, nx)];
        EfieldsY[FLAT(0, j, nx)] = EfieldsY[FLAT(1, j, nx)] ;
        EfieldsY[FLAT(nx-1, j, nx)] = EfieldsY[FLAT(nx-2, j, nx)];
    }
    for(int i=0; i < nx; i++){
        EfieldsX[FLAT(i, ny-1, nx)] = EfieldsX[FLAT(i, ny-2, nx)];
        EfieldsY[FLAT(i, ny-1, nx)] = EfieldsY[FLAT(i, ny-2, nx)];
    }
    for(int elem_i = 0; elem_i<num_of_transmitter_elements; elem_i++){
        const int obs_x = start_x + elem_i * obstacle_spacing;
        EfieldsY[FLAT(obs_x, 0, nx)] = E_magnitude_default* sinf(2.0f * 3.14159265f * frequency_transmitter* time + dphase_elem * (float)(elem_i));
        EfieldsX[FLAT(obs_x, 0, nx)] = 0.0f;
    }

}

void step_time(float* EfieldsX, float* EfieldsY, const Dimensions& dims, const float csq, const float dt){
    const int nx = dims.nx;
    const int ny = dims.ny;
    float *laplaciansX = new float[(nx-2) *( ny-2)];
    float *laplaciansY = new float[(nx-2) *( ny-2)];
    for(int i=1; i< nx-1; i++){
        for(int j=1; j< ny-1; j++){
            const int idx = FLAT(i, j, nx);
            const int lap_idx = FLAT(i-1, j-1, nx-2);
            const float centerX = EfieldsX[idx];
            const float centerY = EfieldsY[idx];
            const float leftX = EfieldsX[FLAT(i-1, j, nx)];
            const float rightX = EfieldsX[FLAT(i+1, j, nx)];
            const float downX = EfieldsX[FLAT(i, j-1, nx)];
            const float upX = EfieldsX[FLAT(i, j+1, nx)];
            const float laplacianX = (leftX + rightX + downX + upX - 4.0f * centerX);
            const float leftY = EfieldsY[FLAT(i-1, j, nx)];
            const float rightY = EfieldsY[FLAT(i+1, j, nx)];
            const float downY = EfieldsY[FLAT(i, j-1, nx)];
            const float upY = EfieldsY[FLAT(i, j+1, nx)];
            const float laplacianY = (leftY + rightY + downY + upY - 4.0f * centerY);
            laplaciansX[FLAT(i-1, j-1, nx-2)] = laplacianX;
            laplaciansY[FLAT(i-1, j-1, nx-2)] = laplacianY;
        }
    }
    for(int i=1; i< nx-1; i++){
        for(int j=1; j< ny-1; j++){
            const int idx = FLAT(i, j, nx);
            const int lap_idx = FLAT(i-1, j-1, nx-2);
            EfieldsX[idx] += csq * laplaciansX[lap_idx] * dt;
            EfieldsY[idx] += csq * laplaciansY[lap_idx] * dt;
        }
    }
}
void get_field_strength_magnitude(const float* EfieldsX, const float* EfieldsY, const Dimensions& dims, float* magnitude_out){
    const int nx = dims.nx;
    const int ny = dims.ny;
    for(int i=0; i< nx; i++){
        for(int j=0; j< ny; j++){
            const int idx = FLAT(i, j, nx);
            const float Ex = EfieldsX[idx];
            const float Ey = EfieldsY[idx];
            magnitude_out[idx] = sqrtf(Ex*Ex + Ey*Ey);
        }
    }
}

#define PRESSURES(i, j, nx, ny) (((i) >= 0) && ((i) < (nx)) && ((j) >= 0) && ((j) < (ny))) ? pressures[FLAT(i, j, nx)] : 0.0f
inline bool WALLS(int i, int j, const int &nx, const int &ny, const std::vector<bool> &walls) { return (((i) >= 0) && ((i) < (nx)) && ((j) >= 0) && ((j) < (ny))) ? walls[FLAT(i, j, nx)] : true; }


