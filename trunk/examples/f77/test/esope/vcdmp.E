      subroutine vcdmp(
c     == entrees ==
     & ov,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcdmp (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : dumper un objet VECTOR
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
c
      integer  ov
      integer  status
c
      integer  i
      integer  length
      integer  size
      integer  ierr
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
c
      size=max(nvcflo,nvcent,nvclog,nvccha)
c
      if(vectyp .eq. 'CHAINE')then
        length=cvccha
      else
        length=1
      endif
c
      write(*,'(1x,a)'    ) 'VECTOR DUMP:'
      write(*,'(1x,a,a)'  ) 'VECTOR DUMP: type  = ',vectyp
      write(*,'(1x,a,i12)') 'VECTOR DUMP: size  = ',size
      write(*,'(1x,a,i12)') 'VECTOR DUMP: len   = ',length
c
      if(vectyp .eq. 'FLOTTANT')then
        do i=1,nvcflo
	  if(vecflo(i) .ne. 0.)then
            write(*,'(1x,a,i12,a,1pg15.7e2)') 
     &      'VECTOR DUMP: value (',i,') = ',vecflo(i)
	  else
            write(*,'(1x,a,i12,a,a)') 
     &      'VECTOR DUMP: value (',i,') = ','         0.    '
	  endif
	enddo
c
      elseif(vectyp .eq. 'ENTIER')then
        do i=1,nvcent
          write(*,'(1x,a,i12,a,i12)') 
     &    'VECTOR DUMP: value (',i,') = ',vecent(i)
	enddo
c
      elseif(vectyp .eq. 'LOGIQUE')then
        do i=1,nvclog
          write(*,'(1x,a,i12,a,l2)') 
     &    'VECTOR DUMP: value (',i,') = ',veclog(i)
	enddo
c
      elseif(vectyp .eq. 'CHAINE')then
        do i=1,nvccha
          write(*,'(1x,a,i12,a,a)') 
     &    'VECTOR DUMP: value (',i,') = ',veccha(i)
	enddo
      endif
c
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcdmp: routine en echec')
      status=1
      return
      end
