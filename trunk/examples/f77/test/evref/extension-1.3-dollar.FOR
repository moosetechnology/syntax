      subroutine gng ( totgrp , nrecgr, course,
     &                   coursm , recgrp, igrec ,
     &                   ngrp   , posgrp, grpenm,
     &                   nmgrp_H, trans , modpil,
     &                   sngrp  , ngrps )

      character*8    modpil, grpenm(ngrp), nmgrp_H,
     &                           sngrp(ngrps)
      integer   ngrp, nrecgr, ngrps
      integer   igrec(nrecgr,2)
      real      recgrp(nrecgr), course, coursm
      real      totgrp
      real      posgrp(ngrp)
      real      trans
C--------+---------+---------+---------+---------+---------+---------+-|
      character*8 group_name, group_name_format
      logical     lg_H$complex, lg$T_mode,
     &            lg_Pi$over
      integer     i, i_group, n_char, ertmod
      real        total, coursn, recgrpw
      character*256 msg$fr, msg$en, err$fr, err$en

      total  = totgrp
      coursn = course - coursm

      if ( modpil .eq. 'T' ) then
        lg$T_mode = .true.
        ertmod = 0
      else
        lg$T_mode = .false.
      endif

      if (lg$T_mode) then
        lg_H$complex = .false.
        do i = 1, nrecgr
          if (grpenm(igrec(i,1)) .eq. nmgrp_H) then
            lg_H$complex = .true.
          endif
        enddo

        if (lg_H$complex) then
          lg$T_mode = .false.
        else
          ertmod = 1
          do i_group = 1, ngrp
            lg_Pi$over = .false.
            do i = 1, ngrps
              if ( grpenm(i_group) .eq. sngrp(i) ) then
                lg_Pi$over = .true.
              endif
            enddo
            if (lg_Pi$over .and.
     &        (grpenm(i_group).ne.nmgrp_H))  then
              lg_Pi$over = .false.
              do i = 1, nrecgr
                if (grpenm(i_group) .eq. grpenm(igrec(i,1))) then
                  lg_Pi$over = .true.
                endif
              enddo
              if (grpenm(i_group) .eq. grpenm(igrec(nrecgr,2))) then
                lg_Pi$over = .true.
              endif
            else
              lg_Pi$over = .true.
            endif
            if (.not.lg_Pi$over) then
              lg$T_mode = .false.
            endif
          enddo
          i_group = 2
          n_char  = 1
        endif

        if (modpil .eq. 'T') then
          if (ertmod .gt. 0) then
            call edwarn(msg$fr,msg$en)
            call edwarn(err$fr,err$en)
          endif
        endif
      endif

      if (.not.lg$T_mode) then
        trans = 0.
      else
        if (total .lt. trans) then
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
        total = total - recgrpw
      enddo

      end
C     end subroutine gng
