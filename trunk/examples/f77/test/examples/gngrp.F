      subroutine gngrp ( totgrp , nrecgr, course,
     &                   coursm , recgrp, igrec ,
     &                   ngrp   , posgrp, grpenm,
     &                   nmgrp_H, trans , modpil,
     &                   sngrp  , ngrps )

C--------+---------+---------+---------+---------+---------+---------+-|
C
C  But:
C     Calculer les positions des groupes en fonction de la valeur du
C     totalisateur d'insertion donne obligatoirement en fraction inseree
C     ( ainsi que toutes les autres grandeurs: course, coursm, recgrp ).
C
C  Entrees:
C
C     totgrp      Totalisateur d'insertion
C     nrecgr      Nombre de recouvrements ( + 1 = nb de groupes )
C     course      Plage maxi de varation de la position des groupes
C     coursm      course morte des groupes
C     recgrp      Recouvrements
C     igrec       Indices des groupes lies dans le tableau posgrp
C     ngrp        Nombre total de groupes ( tableau posgrp )
C     trans       translation ( position du groupe H pour le mode T,
C                 0.0 sinon)
C     modpil      Mode de pilotage
C
C  Sorties:
C
C     posgrp      Position des groupes
C
C--------+---------+---------+---------+---------+---------+---------+-|
C  Auteur:   G.MARTINEZ
C  Date:     20 Oct 93
C  Modification: 31 Mar 94 ( Modif structure Image Coeur )
C  Modification: 15 Dec 94 ( Bug sur la dimension de posgrp )
C  Service : EPNI
C
C  Modifications :
C  -------------
C
C  auteur   : K.Tchao
C  date     : 31/07/07
C  service  : NEPDFS
C  source   : FADE 1990
C  objet    : Gestion du mode T
C
C
C  auteur   : M.Bouxin
C  date     : 14/11/2008
C  service  : NEPDFD
C  source   : FADE 2306
C  objet    : Placer le compteur de chevauchement a une valeur inferieure
C             a la position du groupe H
C
C  auteur   : L. Graziano
C  date     : 15/01/2021
C  service  : DTIDPN-F
C  source   : CMR_6691
C  objet    : Bug correction: P bank positions must be forced to be coherent
C             with H (P insertion >= H insertion) only when the T mode is
C             correctly defined.

C--------+---------+---------+---------+---------+---------+---------+-|
#include <implicit.h>
C--------+---------+---------+---------+---------+---------+---------+-|
      character*8, intent(in)    :: modpil, grpenm(ngrp), nmgrp_H,
     &                              sngrp(ngrps)
      integer,     intent(in)    :: ngrp, nrecgr, ngrps
      integer,     intent(in)    :: igrec(nrecgr,2)
      real,        intent(in)    :: recgrp(nrecgr), course, coursm
      real,        intent(in)    :: totgrp

      real,        intent(out)   :: posgrp(ngrp)
      real,        intent(inout) :: trans
C--------+---------+---------+---------+---------+---------+---------+-|
      character*8 group_name, group_name_format
      logical     lg_H_is_in_complex_group, lg_is_a_correct_T_mode,
     &            lg_is_Pi_and_in_overlapping
      integer     i, i_group, n_char, ertmod
      real        total, coursn, recgrpw
      character*256 msgfr, msgen, errfr, erren
C--------+---------+---------+---------+---------+---------+---------+-|


      total  = totgrp
      coursn = course - coursm

c     Correction si total < posh c'est que on veut sortir les grappes
c     alors que H est tres enfonce (typiquement, situation d'arrete)

c     CMR_6691
      if ( modpil .eq. 'T' ) then
        lg_is_a_correct_T_mode = .true.
c       error index set to 0
        ertmod = 0
      else
        lg_is_a_correct_T_mode = .false.
      endif


      if (lg_is_a_correct_T_mode) then
        lg_H_is_in_complex_group = .false.

        do i = 1, nrecgr
          if (grpenm(igrec(i,1)) .eq. nmgrp_H) then
            lg_H_is_in_complex_group = .true.
          endif
        enddo

        if (grpenm(igrec(nrecgr,2)) .eq. nmgrp_H) then
          lg_H_is_in_complex_group = .true.
        endif


        if (lg_H_is_in_complex_group) then
c          In this case we do not need to check anything. If the users selected the
c          overlappings coherents with the T mode, then P5 will always be less inserted
c          than the other groups. If he didn't, then we are not anymore in T mode, so
c          we do not need to check the T mode coherence.
          lg_is_a_correct_T_mode = .false.
c         error code 1
          ertmod = 1
        else
c          For this to be a true T mode calculation, the groups must
c          be named 'P1', 'P2', 'P3'        , 'P4', and with this order.,
c                or 'P1', 'P2', 'P31', 'P32', 'P4', or something similar.

c         If there is a 'Pi' group that is not in TOTAL (besides the H group),
c         then it is not a correct T mode.
          do i_group = 1, ngrp
c           Is it a 'Pi' group?
            lg_is_Pi_and_in_overlapping = .false.
            do i = 1, ngrps
              if ( grpenm(i_group) .eq. sngrp(i) ) then
                lg_is_Pi_and_in_overlapping = .true.
              endif
            enddo
            if (lg_is_Pi_and_in_overlapping .and.
     &        (grpenm(i_group).ne.nmgrp_H))  then
c             Is it in the complex group?
              lg_is_Pi_and_in_overlapping = .false.
              do i = 1, nrecgr
                if (grpenm(i_group) .eq. grpenm(igrec(i,1))) then
                  lg_is_Pi_and_in_overlapping = .true.
                endif
              enddo
              if (grpenm(i_group) .eq. grpenm(igrec(nrecgr,2))) then
                lg_is_Pi_and_in_overlapping = .true.
              endif
            else
c             If it is not a 'Pi' group or if it is 'H', we do not care.
              lg_is_Pi_and_in_overlapping = .true.
            endif
            if (.not.lg_is_Pi_and_in_overlapping) then
              write(*,*)'  Group '//grpenm(i_group)//
     &                  ' does not belong to the complex group.'
              lg_is_a_correct_T_mode = .false.
c             error code 2
              ertmod = 2
            endif
          enddo


          if (grpenm(igrec(1,1))(1:2) .ne. 'P1') then
            lg_is_a_correct_T_mode = .false.
            write(*,*)'  First group in overlapping: ',
     &                               grpenm(igrec(1,1))
          endif



          i_group = 1
          n_char  = 2
          write(group_name_format, '(A)'), '(A,I1)'

          do i = 2, nrecgr

            if (i .gt. 9) then
              n_char = 3
              write(group_name_format, '(A)'), '(A,I2)'
            endif

            write(group_name, group_name_format), 'P', i_group

c            write(*,*)'i         : ', i
c            write(*,*)'group_name: ', group_name
c            write(*,*)'grpenm    : ', grpenm(igrec(i,1))

            if (grpenm(igrec(i,1))(1:n_char) .ne.
     &          group_name        (1:n_char)) then
              i_group = i_group + 1
              write(group_name, group_name_format), 'P', i_group

c              write(*,*)'group_name: ', group_name
c              write(*,*)'grpenm    : ', grpenm(igrec(i,1))

              if (grpenm(igrec(i,1))(1:n_char) .ne.
     &            group_name        (1:n_char)) then
                lg_is_a_correct_T_mode = .false.
c               a group is not called on the model P...
c               error code 3
                ertmod = 3
c                write(*,*)'lg_is_a_correct_T_mode: ',
c     &                     lg_is_a_correct_T_mode
              endif
            endif
          enddo

          if (nrecgr .gt. 9) then
            n_char = 3
            write(group_name_format, '(A)'), '(A,I2)'
          endif

          write(group_name, group_name_format), 'P', i_group

c          write(*,*)'group_name: ', group_name
c          write(*,*)'grpenm    : ', grpenm(igrec(nrecgr,2))

          if (grpenm(igrec(nrecgr,2))(1:n_char) .ne.
     &        group_name             (1:n_char)) then
            i_group = i_group + 1
            write(group_name, group_name_format), 'P', i_group

c            write(*,*)'group_name: ', group_name
c            write(*,*)'grpenm    : ', grpenm(igrec(i,1))

            if (grpenm(igrec(nrecgr,2))(1:n_char) .ne.
     &          group_name             (1:n_char)) then
              lg_is_a_correct_T_mode = .false.
c             a group is not called on the model P...
c             error code 3
              ertmod = 3
c              write(*,*)'lg_is_a_correct_T_mode: ',
c     &                   lg_is_a_correct_T_mode
            endif
          endif
        endif
c       end if (lg_H_is_in_complex_group) then


        if (modpil .eq. 'T') then
c         not a correct T mode
          if (ertmod .gt. 0) then
c           H in complex group
            if (ertmod .eq. 1) then
              write(msgfr,*) 'Le groupe H est dans le groupe complexe'
              write(msgen,*) 'The H group is in the complex group'
            endif
c           group Pi not in TOTAL besides H
            if (ertmod .eq. 2) then
              write(msgfr,*)
     &            'Un groupe P autre que H n est pas inclus dans TOTAL'
              write(msgen,*)
     &            'A P group is not included in TOTAL besides H'
            endif
c            wrong format for group name
            if (ertmod .eq. 3) then
              write(msgfr,*)
     &              'Un nom de groupe differe de la nomenclature'
              write(msgen,*)
     &              'Group named after a different nomenclature'
            endif
            write(errfr,*) 'Option non compatible avec le mode T'
            write(erren,*) 'Uncompatible option  with T mode'
            call edwarn(msgfr,msgen)
            call edwarn(errfr,erren)
          endif
        endif
        write(*,*)'  lg_H_is_in_complex_group: ',
     &               lg_H_is_in_complex_group
        write(*,*)'  lg_is_a_correct_T_mode  : ',
     &               lg_is_a_correct_T_mode
      endif
c     end if (lg_is_a_correct_T_mode) then




      if (.not.lg_is_a_correct_T_mode) then
c       do not set a minimum insertion, since this is not a T mode or it is a T mode but not correct.
        trans = 0.
      else
        if (total .lt. trans) then
c         In this case the total insertion can not be smaller than the H position.
          write(*,*)'  WARNING: modifying P banks position in order'//
     &              ' to comply with T mode'
          total = trans
        endif
      endif



      do i = 1, nrecgr
        if ( modpil .ne. 'T' ) then
          recgrpw = recgrp(i)
        else
          recgrpw = coursn - recgrp(i)
        endif
        posgrp(igrec(i,1)) = amin1 ( total, coursn )

        if ( (trans              .gt. 0.   ) .and.
     &       (posgrp(igrec(i,1)) .lt. trans) ) then
          write(*,*)'  WARNING: modifying ',grpenm(igrec(i,1)),
     &              ' position in order to comply with T mode'
        endif

        posgrp(igrec(i,1)) = amax1 ( posgrp(igrec(i,1)), trans )
        total = total - recgrpw
      enddo

      posgrp(igrec(nrecgr,2)) = amin1 ( total, coursn )
      if ( (trans                   .gt. 0.   ) .and.
     &     (posgrp(igrec(nrecgr,2)) .lt. trans) ) then
        write(*,*)'  WARNING: modifying ',grpenm(igrec(nrecgr,2)),
     &            ' position in order to comply with T mode'
      endif
      posgrp(igrec(nrecgr,2)) = amax1 ( posgrp(igrec(nrecgr,2)), trans )

C--------+---------+---------+---------+---------+---------+---------+-|
      end subroutine gngrp
