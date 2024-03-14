      SUBROUTINE LIBUNIT(NI)
c
c
      LOGICAL*4 LOGA
      INTEGER   NI
      
      DO I=7,70
        NI=I
        INQUIRE(UNIT=I,OPENED=LOGA)
        IF (.NOT.LOGA) GOTO 100
      ENDDO
      WRITE(6,*)' PAS D''UNITE LOGIQUE LIBRE'
      CALL EXIT (1)
      STOP
  100 CONTINUE
C
      RETURN
      END
