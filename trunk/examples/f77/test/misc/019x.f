C Source: https://heasarc.gsfc.nasa.gov/docs/heasarc/ofwg/docs/general/checksum/node29.html

        subroutine check_encode(sum32,complement,string)

C       sum32     double precision checksum value 
C       complement  (logical) whether to encode the complement of the sum (true)
C                             or encode the sum value itself (false).
C       string  c*16  output ASCII encoded checksum

        double precision sum32,tmpsum,all32
        logical complement
        character*16 string,tmpstr
        integer offset,exclud(13),nbyte(4),ch(4),i,j,k
        integer quot,remain,check,nc

C       all32 is equivalent to a 32 bit unsigned integer with all bits set
        parameter (all32=4.294967295D+09)

C       ASCII 0 is the offset value
        parameter (offset=48)

C       this is the list of ASCII punctuation characters to be excluded
        data exclud/58,59,60,61,62,63,64,91,92,93,94,95,96/

        if (complement)then
C           complement the 32-bit unsigned integer equivalent (flip every bit)
            tmpsum=all32-sum32
        else
C           just encode the sum, not its complement
            tmpsum=sum32
        end if
                           
C       separate each byte of the 32-bit integer into separate 8-bit integers
        nbyte(1)=tmpsum/16777216.
        tmpsum=tmpsum-nbyte(1)*16777216.
        nbyte(2)=tmpsum/65536.
        tmpsum=tmpsum-nbyte(2)*65536.
        nbyte(3)=tmpsum/256.
        nbyte(4)=tmpsum-nbyte(3)*256.

C       encode each 8-bit integer as 4-characters 
        do i=1,4
            quot=nbyte(i)/4+offset
            remain=nbyte(i) - (nbyte(i)/4*4)
            ch(1)=quot+remain
            ch(2)=quot
            ch(3)=quot
            ch(4)=quot

C           avoid ASCII punctuation characters by incrementing and
C           decrementing adjacent characters thus preserving checksum value
10          check=0
            do  k=1,13
                do  j=1,4,2
                    if (ch(j)   .eq. exclud(k) .or.
     &                  ch(j+1) .eq. exclud(k))then
                        ch(j)=ch(j)+1
                        ch(j+1)=ch(j+1)-1
                        check=1
                    end if
                end do
            end do

C           keep repeating, until all punctuation character are removed
            if (check .ne. 0)go to 10

C           convert the byte values to the equivalent ASCII characters
            do j=0,3
                nc=4*j+i
                tmpstr(nc:nc)=char(ch(j+1))
            end do
        end do

C       shift the characters 1 place to the right, since the FITS character
C       string value starts in column 12, which is not word aligned
        string(2:16)=tmpstr(1:15)
        string(1:1)=tmpstr(16:16)

        end


