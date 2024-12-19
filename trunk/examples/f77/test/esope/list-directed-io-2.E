      PROGRAM MAIN
      IMPLICIT NONE
      INTEGER I
      INTEGER J
      INTEGER K
      INTEGER CHAR_INDEX
      INTEGER NCO
      PARAMETER(NCO=10)
      REAL ACOL(NCO)
      CHARACTER*(4) SCOL(NCO)
      CHARACTER*(26) CHAR_LIST
      PARAMETER(CHAR_LIST='ABCDEFGHIJKLMNOPQRSTUVWXYZ')
      CHARACTER*3 SEP
      PARAMETER(SEP='-->')
*     >> inline function of type 'integer -> character*0'
      CHARACTER*0 NILS
      INTEGEROOV,OOO,OO1,OO2,OO3,OO4
      INTEGER*8OOT
      INTEGER NILI
      NILS(K) = ''
*     >> inline function of type 'integer -> integer'
      NILI(K) = 0
      WRITE(6,*)
      WRITE(6,*) 'main: begin'
*     -- initialize ESOPE system
*     >> even, if in that case, it is a pure Fortran-77 test case
      CALL OOOSSG
*     -- initialize arrays
C     do i=1, nco
      DO99001I=1,NCO
        ACOL(I) = SIN(REAL(I))
C     enddo
99001 CONTINUE
C     do i=1, nco
      DO99002I=1,NCO
        SCOL(I) = ' '
C       do j=1, len(scol)
      DO99003J=1,LEN(SCOL)
          CHAR_INDEX =  1 + (LEN(CHAR_LIST) - 1)*ABS(SIN(REAL(I*J)))
          CHAR_INDEX = MIN(LEN(CHAR_LIST), MAX(1, CHAR_INDEX))
          SCOL(I)(J:J) = CHAR_LIST(CHAR_INDEX:CHAR_INDEX)
C       enddo
99003 CONTINUE
C     enddo
99002 CONTINUE
      WRITE(6,*)
      WRITE(6,*) '-- print 1 --'
*     -- print 'acol'
      WRITE(6,'(10(e12.5,a))') ((ACOL(K),' '), K=1,NCO)
      WRITE(6,*)
      WRITE(6,*) '-- print 2 --'
*     -- print 'acol' again with 'sep' as a separator
      WRITE(6,'(10(e12.5,a))') ((ACOL(K),SEP), K=1,NCO)
      WRITE(6,*)
      WRITE(6,*) '-- print 3 --'
*     -- other 'list-directed-I/O'
      WRITE(6,'(10(a,a))') ((' ', SCOL(K)), K=1,10)
      WRITE(6,*)
      WRITE(6,*) '-- print 4 --'
*     >> fails at compile-time:
*     write(6,'(10(a,a))') (('x','y'), k=1,10)
*     >> workaround
      WRITE(6,'(10(a,a))') (('a', 'b' // NILS(K)), K=1,10)
      WRITE(6,*)
      WRITE(6,*) '-- print 5 --'
*     >> fails at run-time:
*     write(6,'(10(i2,i2))') ((11, 22), k=1,10)
*     >> workaround
      WRITE(6,'(10(i2,i2))') ((11, 22 + NILI(K)), K=1,10)
*     -- finalize ESOPE system
      CALL OOOSTP
      WRITE(6,*)
      WRITE(6,*) 'main: end'
      END
