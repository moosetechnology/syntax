      subroutine vcimp(
     & ov,
     & pshp,type,typlen,file,delim,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcimp (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : importer un VECTOR depuis un fichier texte
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  pshp   : shape du VECTOR
cae  type   : type des elements du VECTOR
cae  typlen : longueur du type des elements du VECTOR
cae           N.B. typlen < 0 <=> a determiner d'apres les valeurs
cae  file   : nom de fichier texte
cae  delim  : caracteres delimiteurs
cas  ov     : un VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c 
      integer  length
      external length
c 
      integer  chffo
      external chffo
c 
      integer  chffn
      external chffn
c
#include <vector.obj>
#include <values.seg>
#include <string.seg>
c
      integer       ov
      pointeur      pshp.values
      character*8   type
      integer       typlen
      character*(*) file
      pointeur      delim.string
      integer       status
c
      integer       filunt
      parameter    (filunt=2)
c
      integer       increm
      parameter    (increm=100)
c
      integer       shpsiz
      integer       ierr
      integer       size
      integer       ilin
      integer       icol
      integer       ipos
      integer       ibegin
      integer       iend
      integer       isize
      integer       i
c
      integer            linsiz
      parameter         (linsiz=1024)
      character*(linsiz) line
c
      character*(1) tab
      pointeur      dlm.string
c
      real          xflo
      integer       xent
      logical       xlog
c
      status=0
c
c     -- ouvrir le fichier d'import
      open(unit=filunt,file=file(:length(file,len(file))),
     &status='OLD',access='SEQUENTIAL',form='FORMATTED',
     &iostat=ierr)
      if(ierr.ne.0)then
        call edmess('vcimp: FORTRAN/OPEN en echec')
        goto 999
      endif
c
c     -- creer un VECTOR vide non type
      call vc0new(ov,ierr)
      if(ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR vide non type
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
c
c     -- ouvrir "shape"
      if(pshp .ne. 0)then
        segact,pshp
        shpsiz =pshp.vale(/1)
      else
        shpsiz =0
      endif
c
c     -- si shape fournie, alors verifier shape 
      if(shpsiz .ne. 0)then
        if(shpsiz .ne. 2)then
          call edmess('vcimp: shape pas de dimension 2')
          goto 999
        endif
      endif
c
c     -- si shape fournie, alors dimensionner VECTOR 
      if(shpsiz .ne. 0)then
c
        size=1
	do i=1,shpsiz
	  size=size*pshp.vale(i)
	enddo
c
        if(type .eq. 'FLOTTANT')then
          nvcflo=size
c
        elseif(type .eq. 'ENTIER')then
          nvcent=size
c
        elseif(type .eq. 'LOGIQUE')then
          nvclog=size
c
        elseif(type .eq. 'CHAINE')then
          nvccha=size
        endif
        segadj,tvecto
      endif
c
c     -- typer le VECTOR
      if(type .eq. 'FLOTTANT')then
        vectyp='FLOTTANT'
c
      elseif(type .eq. 'ENTIER')then
        vectyp='ENTIER'
c
      elseif(type .eq. 'LOGIQUE')then
        vectyp='LOGIQUE'
c
      elseif(type .eq. 'CHAINE')then
        vectyp='CHAINE'
	cvccha=max(0,typlen)
        segadj,tvecto
c
      else
        call edmess('vcimp: type inattendu '//type)
	goto 999
      endif
c
c     -- delimiteurs : prendre ceux par defaut ?
      if(delim .eq. 0)then
c       // ceux par defaut
        lstg=3
        segini,dlm
        tab=char(9)
	dlm.cstg=tab//' '//','
      else
c       // ceux fournis
        segini,dlm=delim
        lstg=dlm.cstg(/1)
      endif
c
c     -- lire les elements du VECTOR
      size=0
      ilin=0
 100  continue
c       -- lire une ligne de caracteres
        read(unit=filunt,fmt='(a)',err=998,end=101) line
c
c       -- extraire les elements du VECTOR entre les delimiteurs
        ibegin=1
        icol=0
 110    continue
          if(dlm.cstg(/1) .ge. 1)then
c           >> chffn = CHaracter-Find-First-Not_of
            ibegin=chffn(line,dlm.cstg,ibegin,len(line))
            if(ibegin .eq. 0)then
              goto 100
            else
c             >> chffo = CHaracter-Find-First-Of
              iend=chffo(line,dlm.cstg,ibegin,len(line)) - 1
              iend=max(ibegin,iend)
            endif
          else
            if(ibegin .gt. len(line))then
              goto 100
            else
              iend=len(line)
            endif
          endif
c
c         -- si shape fournie, 
c         -- alors incrementer et verifier {ligne,colonne,position} 
          if(shpsiz .ne. 0)then
            icol=icol+1
            if(icol .eq. 1) ilin=ilin+1
c
            if(.not.(ilin.le.pshp.vale(1) .and. icol.le.pshp.vale(2)))
     &      then
              call edmess('vcimp: shape et fichier incompatibles')
              goto 999
            endif
c
            ipos=ilin + (icol-1)*pshp.vale(1)
          endif
c
          if(type .eq. 'FLOTTANT')then
            read(line(ibegin:iend),*,err=997)xflo
            size=size+1
            if(size .gt. nvcflo)then
              nvcflo=nvcflo+increm
              segadj,tvecto
            endif
c
            if(shpsiz .ne. 0)then
              vecflo(ipos)=xflo
            else
              vecflo(size)=xflo
            endif
c
          elseif(type .eq. 'ENTIER')then
            read(line(ibegin:iend),*,err=997)xent
            size=size+1
            if(size .gt. nvcent)then
              nvcent=nvcent+increm
              segadj,tvecto
            endif
c
            if(shpsiz .ne. 0)then
              vecent(ipos)=xent
            else
              vecent(size)=xent
            endif
c
          elseif(type .eq. 'LOGIQUE')then
            read(line(ibegin:iend),*,err=997)xlog
            size=size+1
            if(size .gt. nvclog)then
              nvclog=nvclog+increm
              segadj,tvecto
            endif
c
            if(shpsiz .ne. 0)then
              veclog(ipos)=xlog
            else
              veclog(size)=xlog
            endif
c
          elseif(type .eq. 'CHAINE')then
            size=size+1
            if(size .gt. nvccha)then
              nvccha=nvccha+increm
              segadj,tvecto
            endif
c
            isize=iend-ibegin+1
            if(typlen.lt.0 .and. isize.gt.cvccha)then
              cvccha=isize
              segadj,tvecto
	    endif
c
            if(shpsiz .ne. 0)then
              veccha(ipos)=line(ibegin:iend)
            else
              veccha(size)=line(ibegin:iend)
            endif
          endif
c
          ibegin=iend+1
        goto 110
c
 101  continue
c
c     -- ajuster la taille du VECTOR
      if(type .eq. 'FLOTTANT')then
	nvcflo=size
        segadj,tvecto
c
      elseif(type .eq. 'ENTIER')then
	nvcent=size
        segadj,tvecto
c
      elseif(type .eq. 'LOGIQUE')then
	nvclog=size
        segadj,tvecto
c
      elseif(type .eq. 'CHAINE')then
	nvccha=size
        segadj,tvecto
      endif
c
c     -- menage
      segsup,dlm
c
c     -- fermer "shape"
      if(pshp .ne. 0) segdes,pshp*NOMOD
c
c     -- fermer le VECTOR
      segdes,tvecto*MOD
      call desstr(ov,'MOD')
c
c     -- fermer le fichier d'import
      close(unit=filunt,status='KEEP',iostat=ierr)
      if(ierr.ne.0)then
        call edmess('vcimp: FORTRAN/CLOSE en echec')
        goto 999
      endif
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 997  continue  
      call edmess('vcimp: FORTRAN/READ interne en echec')
      call edmess('vcimp: line(:) = '//line(ibegin:iend))
      goto 999    
c
 998  continue  
      call edmess('vcimp: FORTRAN/READ en echec')
      goto 999    
c
 999  continue      
      call edmess('vcimp: routine en echec')
      call edmess('vcimp: file = '//file(:length(file,len(file))))
      status=1
      return
      end
