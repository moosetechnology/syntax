      subroutine vcexp(
c     == entrees ==
     & ov,pshp,file,delim,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcexp (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : exporter un VECTOR vers un fichier texte
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  pshp   : shape du VECTOR
cae  file   : nom de fichier texte
cae  delim  : caracteres delimiteurs
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
      integer         ov
      pointeur        pshp.values
      character*(*)   file
      pointeur        delim.string
      integer         status
c
      integer         filunt
      parameter      (filunt=2)
c
      integer            linsiz
      parameter         (linsiz=1024)
      character*(linsiz) line
      integer            linlen
c
      integer            bufsiz
      parameter         (bufsiz=32)
      character*(bufsiz) buf
c
      character*(1) tab
      pointeur      dlm.string
c
      integer         shpsiz
      integer         size
      integer         nl
      integer         nc
      integer         l
      integer         c
      integer         i
      integer         ibegin
      integer         iend
      integer         istep
      integer         kbegin
      integer         kend
      integer         ierr
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le fichier d'import
      open(unit=filunt,file=file(:length(file,len(file))),
     &status='UNKNOWN',access='SEQUENTIAL',form='FORMATTED',
     &iostat=ierr)
      if(ierr.ne.0)then
        call edmess('vcexp: FORTRAN/OPEN en echec')
        goto 999
      endif
c
      rewind(unit=filunt,iostat=ierr)
      if(ierr.ne.0)then
        call edmess('vcexp: FORTRAN/REWIND en echec')
        goto 999
      endif
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
      size=max(nvcflo,nvcent,nvclog,nvccha)
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
          call edmess('vcexp: shape pas de dimension 2')
          goto 999
        endif
c
        if(pshp.vale(1)*pshp.vale(2) .ne. size)then
          call edmess('vcexp: shape et size incompatibles')
          goto 999
        endif
      endif
c
c     -- delimiteurs : prendre ceux par defaut ?
      if(delim .eq. 0)then
c       // ceux par defaut
        lstg=1
        segini,dlm
        tab=char(9)
	dlm.cstg=tab
      else
c       // ceux fournis
        segini,dlm=delim
        lstg=dlm.cstg(/1)
      endif
c
c     -- editer les elements du VECTOR 
      if(shpsiz .ne. 0)then
c       // editer avec shape 
        nl=pshp.vale(1)
        nc=pshp.vale(2)
        do l=1,nl
c
          ibegin = l
          iend   = l+(nc-1)*nl
          istep  = nl
c
	  linlen=0
c
          if(vectyp .eq. 'CHAINE')then
	    do i=ibegin,iend,istep
              line(linlen+1:)=veccha(i)//dlm.cstg
	      linlen=linlen+veccha(/1)+dlm.cstg(/1)
	    enddo
	  else
	    do i=ibegin,iend,istep
              if(vectyp .eq. 'FLOTTANT')then
	        if(vecflo(i) .ne. 0.)then
                  write(buf,'(1pg15.7e2)') vecflo(i)
		else
                  write(buf,'(a)') '         0.    '
		endif
c
              elseif(vectyp .eq. 'ENTIER')then
                write(buf,'(i12)') vecent(i)
c
              elseif(vectyp .eq. 'LOGIQUE')then
                write(buf,'(l2)') veclog(i)
              endif
c
              kbegin=chffn(buf,' ',1,len(buf))
              kend  =chffo(buf,' ',kbegin,len(buf)) - 1
              kend  =max(kbegin,kend)
c	      
              line(linlen+1:)=buf(kbegin:kend)//dlm.cstg
	      linlen=linlen+(kend-kbegin+1)+dlm.cstg(/1)
	    enddo
	  endif
c
          write(filunt,'(a)') line(:linlen)
        enddo
      else
c       // editer sans shape 
        if(vectyp .eq. 'FLOTTANT')then
	  do i=1,nvcflo
	    if(vecflo(i) .ne. 0.)then
              write(filunt,'(1pg15.7e2)') vecflo(i)
	    else
              write(filunt,'(a)') '         0.    '
	    endif
	  enddo
c
        elseif(vectyp .eq. 'ENTIER')then
	  do i=1,nvcent
            write(filunt,'(i12)') vecent(i)
	  enddo
c
        elseif(vectyp .eq. 'LOGIQUE')then
	  do i=1,nvclog
            write(filunt,'(l2)') veclog(i)
	  enddo
c
        elseif(vectyp .eq. 'CHAINE')then
	  do i=1,nvccha
            write(filunt,'(a)') veccha(i)
	  enddo
        endif
      endif
c
c     -- menage
      segsup,dlm
c
c     -- fermer "shape"
      if(pshp .ne. 0) segdes,pshp*NOMOD
c
c     -- fermer le VECTOR
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- fermer le fichier d'import
      close(unit=filunt,status='KEEP',iostat=ierr)
      if(ierr.ne.0)then
        call edmess('vcexp: FORTRAN/CLOSE en echec')
        goto 999
      endif
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcexp: routine en echec')
      call edmess('vcexp: file = '//file(:length(file,len(file))))
      status=1
      return
      end
