      subroutine vcsrt(ov,ov1,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcsrt (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : trier des elements en ordre croissant 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : VECTOR argument
cas  ov     : VECTOR resultat
cas  status : status d'execution (status=0 <=> succes)
c
cm Modification No 1
cm
cm   auteur    : B.Barbier
cm   service   : PSDS
cm   date      : 02/04/2004
cm   objet     : exploitation des nouvelles routines de tri efficace
cm                 rssort, issort, lssort, cssort
cm               les algorithme de tri sont remplaces par des appels
cm               a ces routines
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
#include <string.seg>
c
      integer     ov
      integer     ov1
      integer     status
c
      pointeur    pv.tvecto
      pointeur    pv1.tvecto
c
      integer     ierr
      integer     i
      integer     k
      integer     n
c
      status=0
c
c     -- check version of vector v1
      call vc0ckv(ov1,ierr)
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
c     -- ouvrir le VECTOR v1
      call actstr(ov1)
      pv1=mypnt(ov1,1)
      segact,pv1
      nvcflo=pv1.vecflo(/1)
      nvcent=pv1.vecent(/1)
      nvclog=pv1.veclog(/1)
      nvccha=pv1.veccha(/2)
      cvccha=pv1.veccha(/1)
c
c     -- creer les nouveaux elements
c
      if(pv1.vectyp .eq. 'FLOTTANT')then
        segadj,pv
c       -- recopier tous les elements
	pv.vectyp = pv1.vectyp
        do i=1,nvcflo
          pv.vecflo(i)=pv1.vecflo(i)
	enddo
c       -- trier les elements
        call rssort(nvcflo,pv.vecflo)
c
      elseif(pv1.vectyp .eq. 'ENTIER')then
        segadj,pv
c       -- recopier tous les elements
	pv.vectyp = pv1.vectyp
        do i=1,nvcent
          pv.vecent(i)=pv1.vecent(i)
	enddo
c       -- trier les elements
        call issort(nvcent,pv.vecent)
c
      elseif(pv1.vectyp .eq. 'LOGIQUE')then
        segadj,pv
c       -- recopier tous les elements
	pv.vectyp = pv1.vectyp
        do i=1,nvclog
          pv.veclog(i)=pv1.veclog(i)
	enddo
c       -- trier les elements
        call lssort(nvclog,pv.veclog)
c     
      elseif(pv1.vectyp .eq. 'CHAINE')then
        segadj,pv
c       -- recopier tous les elements
	pv.vectyp = pv1.vectyp
        do i=1,nvccha
          pv.veccha(i)=pv1.veccha(i)
	enddo
c       -- trier les elements 
        call cssort(nvccha,pv.veccha)
        
      endif
c
c     -- fermer le VECTOR v1
      segdes,pv1*NOMOD
      call desstr(ov1,'NOMOD')
c
c     -- fermer le VECTOR v
      segdes,pv*MOD
      call desstr(ov,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcsrt: routine en echec')
      status=1
      return
      end
