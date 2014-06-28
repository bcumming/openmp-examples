#include <iostream>
#include <cmath>

#include <omp.h>

#define _USE_MATH_DEFINES

const int num_steps = 200000000;

int main( void ){
    int i;
    double x, pi, sum, w;
    sum =0;
    pi =0;

    std::cout << "using "
              << omp_get_max_threads()
              << " OpenMP threads" << std::endl;

    w = 1.0/double(num_steps);

    double time = -omp_get_wtime();
    #pragma omp parallel for reduction(+:sum)
    for(int i=0; i<num_steps; ++i) {
        double x = (i+0.5)*w;
        sum = sum + 4.0/(1.0+x*x);
    }

    sum *= w;

    time += omp_get_wtime();

    std::cout << num_steps
              << " steps approximates pi as : "
              << pi
              << ", with relative error "
              << std::fabs(M_PI-pi)/M_PI
              << std::endl;
    std::cout << "took " << time << " seconds" <<std::endl;
}

