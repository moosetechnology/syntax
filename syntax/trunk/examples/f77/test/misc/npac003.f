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
      integer i, ifunc
      common /mycom/ i
      write(6,100) 'Hello Ken'
  100 format(1x,A)
      i = i + 1000
      call mysub
      i = ifunc( i )
      write(6,*) i
      stop
      end

      subroutine mysub
      integer i
      common /mycom/ i
      i = i + 1
      return
      end

      integer function ifunc( k )
      integer k
      ifunc = k * k
      return
      end
   
      block data myblck
      common /mycom/ i
      data i /12345/
      end
C----------------------------------------------------------------------^
