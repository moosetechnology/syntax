      subroutine vcbin(ov,ov1,ov2,oper,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcbin (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : operation binaire sur les elements du VECTOR 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : VECTOR argument
cae  ov2    : VECTOR argument
cae  oper   : operation binaire
cas  ov     : VECTOR resultat
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
c
      integer     ov
      integer     ov1
      integer     ov2
      character*8 oper
      integer     status
c
      pointeur    pv.tvecto
      pointeur    pv1.tvecto
      pointeur    pv2.tvecto
c
      character*8 type
      integer     v1sz
      integer     v2sz
      integer     ierr
      integer     i
c
      status=0
c
c     -- check version of vector v1
      call vc0ckv(ov1,ierr)
      if (ierr.ne.0) goto 999
c
c     -- check version of vector v2
      call vc0ckv(ov2,ierr)
      if (ierr.ne.0) goto 999
c
c     -- creer un VECTOR vide non type
      call vc0new(ov,ierr)
      if(ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR v
      call actstr(ov)
      pv=mypnt(ov,1)
      segact,pv
      nvcflo=pv.vecflo(/1)
      nvcent=pv.vecent(/1)
      nvclog=pv.veclog(/1)
      nvccha=pv.veccha(/2)
      cvccha=pv.veccha(/1)
c
c     -- ouvrir le VECTOR v2
      call actstr(ov2)
      pv2=mypnt(ov2,1)
      segact,pv2
      nvcflo=pv2.vecflo(/1)
      nvcent=pv2.vecent(/1)
      nvclog=pv2.veclog(/1)
      nvccha=pv2.veccha(/2)
      cvccha=pv2.veccha(/1)
      v2sz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- ouvrir le VECTOR v1
      call actstr(ov1)
      pv1=mypnt(ov1,1)
      segact,pv1
      nvcflo=pv1.vecflo(/1)
      nvcent=pv1.vecent(/1)
      nvclog=pv1.veclog(/1)
      nvccha=pv1.veccha(/2)
      cvccha=pv1.veccha(/1)
      v1sz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- verifier la taille de v1 et v2
      if(v1sz .ne. v2sz)then
        call edmess('vcbin: arguments de taille differentes')
	goto 999
      endif
c
c     -- verifier le type de v1 et v2
      if(pv1.vectyp .ne. pv2.vectyp)then
        call edmess('vcbin: arguments de type differents')
	goto 999
      endif
c
c     -- creer les nouveaux elements d'apres l'operation binaire
      if(pv1.vectyp .eq. 'FLOTTANT')then
c
        if(oper .eq. 'ADD')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=pv1.vecflo(i) + pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'MUL')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=pv1.vecflo(i) * pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'SUB')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=pv1.vecflo(i) - pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'DIV')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=pv1.vecflo(i) / pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'MOD')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=mod(pv1.vecflo(i), pv2.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'MIN')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=min(pv1.vecflo(i), pv2.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'MAX')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=max(pv1.vecflo(i), pv2.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'POW')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=pv1.vecflo(i) ** pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'EQ')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecflo(i) .eq. pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'NE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecflo(i) .ne. pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'GT')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecflo(i) .gt. pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'LT')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecflo(i) .lt. pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'GE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecflo(i) .ge. pv2.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'LE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecflo(i) .le. pv2.vecflo(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'ENTIER')then
c
        if(oper .eq. 'ADD')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=pv1.vecent(i) + pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'MUL')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=pv1.vecent(i) * pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'SUB')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=pv1.vecent(i) - pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'DIV')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=pv1.vecent(i) / pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'MOD')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=mod(pv1.vecent(i), pv2.vecent(i))
	  enddo
c
        elseif(oper .eq. 'MIN')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=min(pv1.vecent(i), pv2.vecent(i))
	  enddo
c
        elseif(oper .eq. 'MAX')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=max(pv1.vecent(i), pv2.vecent(i))
	  enddo
c
        elseif(oper .eq. 'POW')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=pv1.vecent(i) ** pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'EQ')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecent(i) .eq. pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'NE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecent(i) .ne. pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'GT')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecent(i) .gt. pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'LT')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecent(i) .lt. pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'GE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecent(i) .ge. pv2.vecent(i)
	  enddo
c
        elseif(oper .eq. 'LE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.vecent(i) .le. pv2.vecent(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'LOGIQUE')then
c
        if(oper .eq. 'AND')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.veclog(i) .and. pv2.veclog(i)
	  enddo
c
        elseif(oper .eq. 'OR')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.veclog(i) .or. pv2.veclog(i)
	  enddo
c
        elseif(oper .eq. 'EQV')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.veclog(i) .eqv. pv2.veclog(i)
	  enddo
c
        elseif(oper .eq. 'NEQV')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.veclog(i) .neqv. pv2.veclog(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'CHAINE')then
c
        if(oper .eq. 'EQ')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.veccha(i) .eq. pv2.veccha(i)
	  enddo
c
        elseif(oper .eq. 'NE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=pv1.veccha(i) .ne. pv2.veccha(i)
	  enddo
c
        elseif(oper .eq. 'GT')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=lgt(pv1.veccha(i),pv2.veccha(i))
	  enddo
c
        elseif(oper .eq. 'LT')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=llt(pv1.veccha(i),pv2.veccha(i))
	  enddo
c
        elseif(oper .eq. 'GE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=lge(pv1.veccha(i),pv2.veccha(i))
	  enddo
c
        elseif(oper .eq. 'LE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = 'LOGIQUE'
c
	  do i=1,nvclog
            pv.veclog(i)=lle(pv1.veccha(i),pv2.veccha(i))
	  enddo
c
        elseif(oper .eq. 'CAT')then
          cvccha=pv1.veccha(/1) + pv2.veccha(/1)
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvccha
            pv.veccha(i)=pv1.veccha(i)//pv2.veccha(i)
	  enddo
	else
          goto 998
	endif
      endif
c
c     -- fermer le VECTOR v1
      segdes,pv1*NOMOD
      call desstr(ov1,'NOMOD')
c
c     -- fermer le VECTOR v2
      segdes,pv2*NOMOD
      call desstr(ov2,'NOMOD')
c
c     -- fermer le VECTOR v
      segdes,pv*MOD
      call desstr(ov,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 998  continue      
        type=pv1.vectyp
        call edmess('vcbin: operation non disponible sur ce type')
        call edmess('vcbin: operation = '//oper)
        call edmess('vcbin:      type = '//type)
      goto 999
c
 999  continue      
      call edmess('vcbin: routine en echec')
      status=1
      return
      end
