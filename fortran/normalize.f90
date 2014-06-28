program normalize

use omp_lib
implicit none

    integer :: n = 10000000
    real(kind=8), allocatable :: v(:)
    logical :: validated = .false.
    integer :: max_threads
    real(kind=8) :: time_serial, time_parallel

    allocate(v(n))
    call initialize(v, n)
    time_serial = -omp_get_wtime()
    call normalize_vector(v, n);
    time_serial = time_serial + omp_get_wtime()

    ! chck the answer
    print *, "serial error   : " , abs(norm(v, n) - 1.0d0)

    max_threads = omp_get_max_threads();
    call initialize(v, n);
    time_parallel = -omp_get_wtime();
    call normalize_vector_omp(v, N);
    time_parallel = time_parallel + omp_get_wtime();

    ! chck the answer
    print *, 'parallel error : ', abs(norm(v,n) - 1.0d0)

    print *, 'threads    : ', max_threads
    print *, 'serial     : ', time_serial
    print *, 'parallel   : ', time_parallel
    print *, 'speedup    : ', time_serial/time_parallel
    print *, 'efficiency : ', (time_serial/time_parallel)/max_threads

    deallocate(v)

contains

! function to compute the 2-norm of a vector v of length n
real (kind=8) function norm(v, n)
    implicit none
    integer,      intent(in) :: n
    real(kind=8), intent(out) :: v(n)

    integer :: i

    !$omp parallel do reduction(+:norm)
    do i=1,n
        norm = norm + v(i)*v(i)
    end do
    !$omp end parallel do

    norm = sqrt(norm)
end

! initialise v to values between -10 and 10
subroutine initialize(v, n)
    implicit none
    integer,      intent(in) :: n
    real(kind=8), intent(out) :: v(n)

    integer :: i

    !$omp parallel do
    do i=1,n
        v(i) = cos(1.0d0*i) * 10.
    end do
    !$omp end parallel do
end subroutine

subroutine normalize_vector(v, n)
    implicit none
    integer,      intent(in) :: n
    real(kind=8), intent(out) :: v(n)

    integer :: i
    real(kind=8) :: norm = 0.0d0

    ! compute the norm of v
    do i=1,n
        norm = norm + v(i)*v(i);
    end do
    norm = sqrt(norm)

    ! normalize v
    do i=1,n
        v(i) = v(i) / norm
    end do
end

subroutine normalize_vector_omp(v, n)
    implicit none
    integer,      intent(in) :: n
    real(kind=8), intent(out) :: v(n)

    integer :: i
    real(kind=8) :: norm = 0.0d0

    ! compute the norm of v
    !$omp parallel

    !$omp do reduction(+:norm)
    do i=1,n
        norm = norm + v(i)*v(i);
    end do
    !$omp end do

    !$omp single
    norm = sqrt(norm)
    !$omp end single

    ! normalize v
    !$omp do
    do i=1,n
        v(i) = v(i) / norm
    end do
    !$omp end do

    !$omp end parallel
end

end

