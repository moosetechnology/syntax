C NPAC F77 EXAMPLE 001, SHOWING SOURCE CODE LAYOUT.
C        1         2         3         4         5         6         7
C---------------THIS IS A COMMENT---------------------------------------V
Col. 1    : Blank, or a "c" or "*" for comments
Col. 1-5  : Statement label (optional)
Col. 6    : Continuation of previous line (optional)
Col. 7-72 : Statements
Col. 73-80: Sequence number (optional, rarely used today)
C Program Layout:
C----------------------------------------------------------------------+
C Block data, function program, subroutine                             |
C-----------------+----------------+-----------------------------------+
C FORMAT          | PARAMETER      | IMPLICIT                          |
C                 |                +-----------------------------------+
C                 |                | type declarations and other spec  |
C                 |                | statement: COMMON, DIMENSION, SAVE|
C                 |                | EQUIVALENCE, EXTERNAL, INTRINSIC  |
C                 +----------------+-----------------------------------+ 
C                 | DATA           | statement functions               |  
C                 |                +-----------------------------------+
C                 |                | executable statements             |
C-----------------+----------------+-----------------------------------+                
C END                                                                  |  
C----------------------------------------------------------------------+
C234567890123456789012345678901234567890123456789012345678901234567890123
      PROGRAM myprog
C     p
C     r
C     o
C     g
C     r
C     a
C     m
C     m
C     y
C     p
C     r
C     o
C     g
      INTEGER i, ifunc
      COMMON /mycom/ i
      WRITE (6, 100) 'Hello Ken'
100   FORMAT (1X, A)
C     f
C     o
C     r
C     m
C     a
C     t
C     (
C     1
C     x
C     ,
C     A
C     )
      i = i+1000
      CALL mysub
      i = ifunc (i)
      WRITE (6, *) i
      STOP
      END
      SUBROUTINE mysub
      INTEGER i
      COMMON /mycom/ i
      i = i+1
      RETURN
      END
      INTEGER FUNCTION ifunc (k)
      INTEGER k
      ifunc = k*k
      RETURN
      END
      BLOCK DATA myblck
      COMMON /mycom/ i
      DATA i /12345/
      END
C----------------------------------------------------------------------^
