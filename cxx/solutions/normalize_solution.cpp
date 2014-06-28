#include <iostream>
#include <cstdlib>
#include <cmath>

#include <omp.h>

// function to compute the 2-norm of a vector v of length n
template <typename T>
T norm(T *v, int n){
    T norm = 0.;

    #pragma omp parallel for reduction(+:norm)
    for(int i=0; i<n; i++)
        norm += v[i]*v[i];

    return sqrt(norm);
}

// initialise v to values between -10 and 10
template <typename T>
void initialize(T *v, int n){
    #pragma omp parallel for
    for(int i=0; i<n; i++)
        v[i] = cos(T(i)) * 10.;
}


template <typename T>
void normalize_vector(T *v, int n){
    T norm = 0.;

    // compute the norm of v
    for(int i=0; i<n; i++)
        norm += v[i]*v[i];
    norm = sqrt(norm);

    // normalize v
    for(int i=0; i<n; i++)
        v[i] /= norm;
}

template <typename T>
void normalize_vector_ompV1(T *v, int n){
    T sum = 0.;

    #pragma omp parallel for reduction(+:sum)
    for(int i=0; i<n; i++){
        sum += v[i]*v[i];
    }

    T nrm = sqrt(sum);

    #pragma omp parallel for
    for(int i=0; i<n; i++)
        v[i] /= nrm;
}

template <typename T>
void normalize_vector_ompV2(T *v, int n){
    T sum = 0.;

    #pragma omp parallel
    {
        #pragma omp for reduction(+:sum)
        for(int i=0; i<n; i++){
            sum += v[i]*v[i];
        }

        T nrm = sqrt(sum);

        #pragma omp for
        for(int i=0; i<n; i++)
            v[i] /= nrm;
    }
}

template <typename T>
void normalize_vector_ompV3(T *v, int n){
    T sum_global = 0.;
    #pragma omp parallel
    {
        T sum = 0.;

        #pragma omp for
        for(int i=0; i<n; i++){
            sum += v[i]*v[i];
        }

        #pragma omp critical
        {
            sum_global += sum;
        }

        T nrm = sqrt(sum_global);

        #pragma omp for
        for(int i=0; i<n; i++)
            v[i] /= nrm;
    }
}

int main( void ){
    const int N = 40000000;
    double *v = (double*)malloc(N*sizeof(double));
    bool validated = false;

    initialize(v, N);
    double time_serial = -omp_get_wtime();
    normalize_vector(v, N);
    time_serial += omp_get_wtime();

    // check the answer
    std::cout << "serial error   : " << fabs(norm(v,N) - 1.) << std::endl;

    initialize(v, N);
    double time_parallel = -omp_get_wtime();
    normalize_vector_ompV1(v, N);
    time_parallel += omp_get_wtime();

    // check the answer
    std::cout << "parallel error : " << fabs(norm(v,N) - 1.) << std::endl;

    int max_threads = omp_get_max_threads();
    std::cout <<  max_threads << " threads" << std::endl;
    std::cout << "serial     : " << time_serial << std::endl;
    std::cout << "parallel   : " << time_parallel << std::endl;
    std::cout << "speedup    : " << time_serial/time_parallel << std::endl;
    std::cout << "efficiency : " << (time_serial/time_parallel)/double(max_threads) << std::endl;

    free(v);
    return 0;
}


