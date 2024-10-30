      subroutine vcacc(
c     == entrees ==
     & ov,oper,
c     == sorties ==
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcacc (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : accumuler les elements du VECTOR 
cb               par une operation binaire
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  oper   : operation binaire
cas  pval   : resultat dans un conteneur "values"
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
#include <values.seg>
c
      integer     ov
      character*8 oper
      pointeur    pval.values
      integer     status
c
      integer       ierr
      character*8   vatp
      integer       i
c
      double precision xflo
      integer          xent
      logical          xlog
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
c
c     -- creer a vide un conteneur "values"
      vatp=vectyp
      call valnew(pval,vatp,ierr)
      if(ierr .ne. 0) goto 999
c
c     -- ouvrir le conteneur "values"
      segact,pval
      valnf =pval.valf(/1)
      valne =pval.vale(/1)
      valnl =pval.vall(/1)
      valnc =pval.valc(/2)
      valsmx=pval.valc(/1)
c
c     -- reduire les elements d'apres l'operation binaire
      if(vectyp .eq. 'FLOTTANT')then
c
        if(oper .eq. 'ADD')then
          xflo=0.
	  do i=1,nvcflo
            xflo=xflo+vecflo(i)
	  enddo
c
        elseif(oper .eq. 'MUL')then
          xflo=1.
	  do i=1,nvcflo
            xflo=xflo*vecflo(i)
	  enddo
c
        elseif(oper .eq. 'MIN')then
	  if(nvcflo .ge. 1)then
            xflo=vecflo(1)
	  else
            xflo=0.
	  endif
c
	  do i=2,nvcflo
            xflo=min(xflo,dble(vecflo(i)))
	  enddo
c
        elseif(oper .eq. 'MAX')then
	  if(nvcflo .ge. 1)then
            xflo=vecflo(1)
	  else
            xflo=0.
	  endif
c
	  do i=2,nvcflo
            xflo=max(xflo,dble(vecflo(i)))
	  enddo
	else
          goto 998
	endif
c
        valnf=1
	segadj,pval
        pval.valf(valnf)=xflo
c
      elseif(vectyp .eq. 'ENTIER')then
c
        if(oper .eq. 'ADD')then
          xent=0
	  do i=1,nvcent
            xent=xent+vecent(i)
	  enddo
c
        elseif(oper .eq. 'MUL')then
          xent=1
	  do i=1,nvcent
            xent=xent*vecent(i)
	  enddo
c
        elseif(oper .eq. 'MIN')then
	  if(nvcent .ge. 1)then
            xent=vecent(1)
	  else
            xent=0
	  endif
c
	  do i=2,nvcent
            xent=min(xent,vecent(i))
	  enddo
c
        elseif(oper .eq. 'MAX')then
	  if(nvcent .ge. 1)then
            xent=vecent(1)
	  else
            xent=0
	  endif
c
	  do i=2,nvcent
            xent=max(xent,vecent(i))
	  enddo
	else
          goto 998
	endif
c
        valne=1
	segadj,pval
        pval.vale(valne)=xent
c
      elseif(vectyp .eq. 'LOGIQUE')then
c
        if(oper .eq. 'AND')then
          xlog=.true.
	  do i=1,nvclog
            xlog=xlog .and. veclog(i)
	  enddo
c
        elseif(oper .eq. 'OR')then
          xlog=.false.
	  do i=1,nvclog
            xlog=xlog .or. veclog(i)
	  enddo
	else
          goto 998
	endif
c
        valnl=1
	segadj,pval
        pval.vall(valnl)=xlog
c
      elseif(vectyp .eq. 'CHAINE')then
        goto 998
      endif
c
c     -- fermer le conteneur "values"
      segdes,pval
c
c     -- fermer le VECTOR
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 998  continue      
        call edmess('vcacc: operation non disponible sur ce type')
        call edmess('vcacc: operation = '//oper)
        call edmess('vcacc:      type = '//vatp)
      goto 999
c
 999  continue      
      call edmess('vcacc: routine en echec')
      status=1
      return
      end
