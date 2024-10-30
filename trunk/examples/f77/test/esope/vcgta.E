      subroutine vcgta(
c     == entrees ==
     & ov,
c     == sorties ==
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcgta (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : recuperer les valeurs de tous les elements
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cas  pval   : un conteneur "values"
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
      integer  ov
      pointeur pval.values
      integer  status
c
      integer       ierr
      character*8   vatp
      integer       i
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
c     -- charger les elements dans le conteneur "values"
      if(vectyp .eq. 'FLOTTANT')then
        valnf=nvcflo
	segadj,pval
	do i=1,nvcflo
          pval.valf(i)=vecflo(i)
	enddo
c
      elseif(vectyp .eq. 'ENTIER')then
        valne=nvcent
	segadj,pval
	do i=1,nvcent
          pval.vale(i)=vecent(i)
	enddo
c
      elseif(vectyp .eq. 'LOGIQUE')then
        valnl=nvclog
	segadj,pval
	do i=1,nvclog
          pval.vall(i)=veclog(i)
	enddo
c
      elseif(vectyp .eq. 'CHAINE')then
	valsmx=cvccha
        valnc=nvccha
	segadj,pval
	do i=1,nvccha
          pval.valc(i)=veccha(i)
          pval.vals(i)=cvccha
	enddo
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
 999  continue      
      call edmess('vcgta: routine en echec')
      status=1
      return
      end
