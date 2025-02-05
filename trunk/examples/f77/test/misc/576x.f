c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: mpi_stubs/mpi_stubs_f77.h
c
c
c  Dummy parameters for MPI F77 stubs
c
      integer MPI_COMM_WORLD
      parameter ( MPI_COMM_WORLD = 0 )
c
c  Return values.
c
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )
c
c  recv message status
c
      integer mpi_status_size
      parameter ( mpi_status_size = 3 )
      integer mpi_source
      parameter ( mpi_source = 1 )
      integer mpi_tag
      parameter ( mpi_tag = 2 )
      integer mpi_count
      parameter ( mpi_count = 3 )
c
c  recv flags
c
      integer mpi_any_source
      parameter ( mpi_any_source = -1 )
      integer mpi_any_tag
      parameter ( mpi_any_tag = -1 )
c
c  data types and sizes
c
      integer mpi_integer
      parameter ( mpi_integer = 1 )
      integer mpi_real
      parameter ( mpi_real = 2 )
      integer mpi_double_precision
      parameter ( mpi_double_precision = 3 )
      integer mpi_logical
      parameter ( mpi_logical = 4 )
      integer mpi_character
      parameter ( mpi_character = 5 )
c
c  allreduce operations
c
      integer mpi_sum
      parameter ( mpi_sum = 1 )
      integer mpi_max
      parameter ( mpi_max = 2 )
      integer mpi_min
      parameter ( mpi_min = 3 )
      integer mpi_product
      parameter ( mpi_product = 4 )
c
c  timer
c
      double precision mpi_wtime
