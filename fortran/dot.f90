program dot

use omp_lib

implicit none

integer      :: N = 100000000
integer      :: i, num_threads
real(kind=8), dimension(:), allocatable  :: a, b
real(kind=8) :: time, result, expected

allocate(a(N), b(N))

num_threads = omp_get_max_threads()
print *, 'dot of vectors with length ', N, ' with ', num_threads, ' threads'

!$omp parallel do
do i=1,N
    a(i) = 1.0/2.0
    b(i) = i
enddo
!$omp end parallel do

time = -omp_get_wtime();

print *, 'took ', time, ' seconds'

deallocate(a, b)

end ! program dot

