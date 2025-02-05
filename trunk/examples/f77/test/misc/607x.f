c  Source: https://github.com/johannesgerer/jburkardt-f77
c  File name: openmp_stubs/omp_lib.h
c
#include "omp_lib_kinds.h"

      integer openmp_version
      parameter ( openmp_version = 201107 )

      external omp_set_num_threads
      integer omp_get_num_threads
      external omp_get_num_threads
      integer omp_get_max_threads
      external omp_get_max_threads
      integer omp_get_thread_num
      external omp_get_thread_num
      integer omp_get_num_procs
      external omp_get_num_procs
      logical omp_in_parallel
      external omp_in_parallel
      external omp_set_dynamic
      logical omp_get_dynamic
      external omp_get_dynamic
      external omp_set_nested
      logical omp_get_nested
      external omp_get_nested
      external omp_set_schedule
      external omp_get_schedule
      integer omp_get_thread_limit
      external omp_get_thread_limit
      external omp_set_max_active_levels
      integer omp_get_max_active_levels
      external omp_get_max_active_levels
      integer omp_get_level
      external omp_get_level
      integer omp_get_ancestor_thread_num
      external omp_get_ancestor_thread_num
      integer omp_get_team_size
      external omp_get_team_size
      integer omp_get_active_level
      external omp_get_active_level
      logical omp_in_final
      external omp_in_final
      external omp_init_lock
      external omp_destroy_lock
      external omp_set_lock
      external omp_unset_lock
      logical omp_test_lock
      external omp_test_lock
      external omp_init_nest_lock
      external omp_destroy_nest_lock
      external omp_set_nest_lock
      external omp_unset_nest_lock
      integer omp_test_nest_lock
      external omp_test_nest_lock
      double precision omp_get_wtick
      external omp_get_wtick
      double precision omp_get_wtime
      external omp_get_wtime
