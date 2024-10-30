C Source: https://www.ibm.com/docs/en/pessl/5.3.0?topic=programs-sample-utilities-provided-parallel-essl

      SUBROUTINE DESYM(NROW,A,JA,IA,AS,JAS,IAS,IAW,WORK,NNZERO,
     +   VALUE)
      IMPLICIT NONE
C      .. Scalar Arguments ..
      INTEGER NROW,NNZERO,VALUE,INDEX
C     .. Array Arguments ..
      DOUBLE PRECISION A(*),AS(*),WORK(*)
      INTEGER IA(*),IAS(*),JAS(*),JA(*),IAW(*)
C     .. Local Scalars ..
      INTEGER I,IAW1,IAW2,IAWT,J,JPT,K,KPT,LDIM,COUNT,JS,BUFI
C      REAL*8  BUF
C     ..
 
      DO I=1,NROW
         IAW(I)=0
      END DO
C    ....Compute element belonging to each row in output matrix.....
      DO I=1,NROW
         DO J=IA(I),IA(I+1)-1
            IAW(I)=IAW(I)+1
            IF (JA(J).NE.I) IAW(JA(J))=IAW(JA(J))+1
         END DO
      END DO
 
      IAS(1)=1
      DO I=1,NROW
         IAS(I+1)=IAS(I)+IAW(I)
         IAW(I)=0
      END DO
 
 
C
C     .....Computing values array AS and column array indices JAS....
C
      DO I=1,NROW
         DO J=IA(I),IA(I+1)-1
            IF (VALUE.NE.0) THEN
               AS(IAS(I)+IAW(I))=A(J)
            ENDIF
            JAS(IAS(I)+IAW(I))=JA(J)
            IAW(I)=IAW(I)+1
            IF (I.NE.JA(J)) THEN
               IF (VALUE.NE.0) THEN
                  AS(IAS(JA(J))+IAW(JA(J)))=A(J)
               ENDIF
               NNZERO=NNZERO+1
               JAS(IAS(JA(J))+IAW(JA(J)))=I
               IAW(JA(J))=IAW(JA(J))+1
            END IF
         END DO
      END DO
 
C     ......Sorting output arrays by column index......
C     .....the IAS index not must be modified.....
C
      DO I=1,NROW
         CALL ISORTX(JAS(IAS(I)),1,IAS(I+1)-IAS(I),IAW)
         INDEX=IAS(I)-1
         IF (VALUE.NE.0) THEN
            DO J=1,IAS(I+1)-IAS(I)
               WORK(J)=AS(IAW(J)+INDEX)
            END DO
            DO J=1,IAS(I+1)-IAS(I)
               AS(J+INDEX)=WORK(J)
            END DO
         ENDIF
C         ....column indices are already sorted by ISORTX...
      ENDDO
      RETURN
 
      END

