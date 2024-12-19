c     Pierre Boullier (dec 2024): test "DOUBLE COMPLEX"

      implicit double complex ( a-h, o-z )
      
      double complex   A( LDA, * ), B( LDB, * ), C( LDC, * )
      double complex fxvec ( n_max )
      double complex w(1:m+1)

c     Ici, "double" semble inutile!!
      double complex*16   A( LDA, * ), B( LDB, * ), C( LDC, * )
      complex*16   A( LDA, * ), B( LDB, * ), C( LDC, * )

       double complex A(MDA,MDA), ANOISE, B(MDA), DUMMY, GEN, H(MDA)
       double complex ONE, SM, SMALL, SRSMSQ, ZERO
      END
