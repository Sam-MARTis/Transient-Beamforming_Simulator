#define NX 32
#define NY 32





__global__ void pressure_solver_kernel(float* pressure, const float* divergence, const int nx,const int ny, const int marker){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    const int idx = j * nx + i;
    const int idx_marker = i+j;
    const bool correct_cell_is = (idx_marker & 1)

    // const bool is_ = (idx_marker & 1) == marker;
    // if(is_odd){
        
    // } else {
    //     // Apply pressure solver for even markers
    // }.0
    
}

void pressure_solver_kernel_launcher(float* pressure, const float* divergence, const int nx,const int ny, const int iterations) {
    dim3 blockSize(16, 16);
    dim3 gridSize((nx + blockSize.x - 1) / blockSize.x, (ny + blockSize.y - 1) / blockSize.y);
    for(int i=0; i<iterations; i++){
    pressure_solver_kernel<<<gridSize, blockSize>>>(pressure, divergence, nx, ny, 0);
    pressure_solver_kernel<<<gridSize, blockSize>>>(pressure, divergence, nx, ny, 1);
    }

}
