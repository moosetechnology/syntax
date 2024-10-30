c begin "user.seg"
c ubb(ubbcnt) : ordinal positions of books borrowed by the user 
      integer ubbcnt
      segment, user
       character*40 uname
       integer ubb(ubbcnt)
      end segment
c end "user.seg"
