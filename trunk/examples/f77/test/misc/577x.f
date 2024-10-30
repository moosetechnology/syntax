c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: openmp_stubs/omp_lib_kinds.h
c
c  omp_lib_kinds.h
c
      integer omp_sched_static
      parameter ( omp_sched_static = 1 )
      integer omp_sched_dynamic
      parameter ( omp_sched_dynamic = 2 )
      integer omp_sched_guided
      parameter ( omp_sched_guided = 3 )
      integer omp_sched_auto
      parameter ( omp_sched_auto = 4 )
