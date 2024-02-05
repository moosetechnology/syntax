Hubert Garavel - Wed Jan 18 18:10:24 CET 2023

Warnings of GCC when compiling CSYNT
====================================

xcsynt/src/csynt.c: In function 'csynt_run':
xcsynt/src/csynt.c:244:5: warning: this statement may fall through [-Wimplicit-fallthrough=]
  if (is_force) {
     ^
xcsynt/src/csynt.c:248:5: note: here
     default:
     ^~~~~~~
xcsynt/src/csynt.c: In function 'get_constructor_name':
xcsynt/src/csynt.c:196:18: warning: '%ld' directive writing between 1 and 10 bytes into a region of size 9 [-Wformat-overflow=]
  sprintf (H, "R (%ld) ", (long)h);
                  ^~~
xcsynt/src/csynt.c:196:14: note: directive argument in the range [1, 2147483647]
  sprintf (H, "R (%ld) ", (long)h);
              ^~~~~~~~~~
xcsynt/src/csynt.c:196:2: note: 'sprintf' output between 7 and 16 bytes into a destination of size 12
  sprintf (H, "R (%ld) ", (long)h);
  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
xcsynt/src/csynt.c:199:20: warning: ')' directive writing 1 byte into a region of size between 0 and 9 [-Wformat-overflow=]
  sprintf (K, " (%ld)", (long)k);
                    ^
xcsynt/src/csynt.c:199:2: note: 'sprintf' output between 5 and 14 bytes into a destination of size 12
  sprintf (K, " (%ld)", (long)k);
  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

xcsynt/src/LR0.c: In function 'sornt':
xcsynt/src/LR0.c:602:26: warning: '%ld' directive writing between 1 and 10 bytes into a region of size 6 [-Wformat-overflow=]
      sprintf (s1, " ---> %ld", (long)(next_state < 0 ? 0 : next_state));
                          ^~~
xcsynt/src/LR0.c:602:19: note: directive argument in the range [0, 2147483647]
      sprintf (s1, " ---> %ld", (long)(next_state < 0 ? 0 : next_state));
                   ^~~~~~~~~~~
xcsynt/src/LR0.c:602:6: note: 'sprintf' output between 8 and 17 bytes into a destination of size 12
      sprintf (s1, " ---> %ld", (long)(next_state < 0 ? 0 : next_state));
      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
xcsynt/src/LR0.c: In function 'sort':
xcsynt/src/LR0.c:627:23: warning: '%ld' directive writing between 1 and 10 bytes into a region of size 6 [-Wformat-overflow=]
   sprintf (s1, " ---> %ld", (long)(next_state < 0 ? 0 : next_state));
                       ^~~
xcsynt/src/LR0.c:627:16: note: directive argument in the range [0, 2147483647]
   sprintf (s1, " ---> %ld", (long)(next_state < 0 ? 0 : next_state));
                ^~~~~~~~~~~
xcsynt/src/LR0.c:627:3: note: 'sprintf' output between 8 and 17 bytes into a destination of size 12
   sprintf (s1, " ---> %ld", (long)(next_state < 0 ? 0 : next_state));
   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

xcsynt/src/ARC.c: In function 'compute_back':
xcsynt/src/ARC.c:861:10: warning: this statement may fall through [-Wimplicit-fallthrough=]
          {
          ^
xcsynt/src/ARC.c:899:6: note: here
      case -1 : /* UNBOUNDED_ sans knots */
      ^~~~
xcsynt/src/ARC.c: In function 'print_pss':
xcsynt/src/ARC.c:2173:16: warning: '%ld' directive writing between 1 and 10 bytes into a region of size 8 [-Wformat-overflow=]
   sprintf (s, "%ld", (long)*bot);
                ^~~
xcsynt/src/ARC.c:2173:15: note: directive argument in the range [0, 2147483647]
   sprintf (s, "%ld", (long)*bot);
               ^~~~~
xcsynt/src/ARC.c:2173:3: note: 'sprintf' output between 2 and 11 bytes into a destination of size 8
   sprintf (s, "%ld", (long)*bot);
   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

xcsynt/src/messages_mngr.c:177:14: warning: 'items_index' defined but not used [-Wunused-function]
 static SXINT items_index (SXINT cur_index, SXINT top_index, SXINT *nb)
              ^~~~~~~~~~~

In file included from xcsynt/incl/csynt_optim.h:43,
                 from xcsynt/src/optim_lo.c:27:
xcsynt/src/optim_lo.c: In function 'output_fe':
incl/put_edit.h:57:15: warning: this statement may fall through [-Wimplicit-fallthrough=]
      _SXPE_col+=strlen(_SXPE_string))
      ~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~
incl/put_edit.h:60:7: note: in expansion of macro 'put_edit_apnnl'
       put_edit_apnnl(string))
       ^~~~~~~~~~~~~~
xcsynt/src/optim_lo.c:419:2: note: in expansion of macro 'put_edit_nnl'
  put_edit_nnl (30, "*");
  ^~~~~~~~~~~~
xcsynt/src/optim_lo.c:421:5: note: here
     case ScanShift:
     ^~~~
xcsynt/src/optim_lo.c: In function 'xt_to_str':
xcsynt/src/optim_lo.c:86:35: warning: 'sprintf' may write a terminating nul past the end of the destination [-Wformat-overflow=]
      sprintf (small_string, " &%ld", (long)prdct);
                                   ^
xcsynt/src/optim_lo.c:86:6: note: 'sprintf' output between 4 and 13 bytes into a destination of size 12
      sprintf (small_string, " &%ld", (long)prdct);
      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

xcsynt/src/left_corner.c: In function 'csynt_run':
xcsynt/src/left_corner.c:203:5: warning: this statement may fall through [-Wimplicit-fallthrough=]
  if (is_force) {
     ^
xcsynt/src/left_corner.c:207:5: note: here
     default:
     ^~~~~~~

xcsynt/src/LC0.c: In function 'ntfe':
xcsynt/src/LC0.c:598:16: warning: variable 'is_xnt' set but not used [-Wunused-but-set-variable]
     bool  is_xnt, is_nd_list;
                ^~~~~~
	gcc -o xcsynt/lc.out


Fatal errors of Sun C (5.12 SunOS_i386 2011/11/16) when compiling CSYNT
=======================================================================


"xcsynt/src/LR0.c", line 118: warning: argument #7 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxY.h", line 33
	argument : pointer to void

"xcsynt/src/LALR1.c", line 1837: warning: argument #7 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxY.h", line 33
	argument : pointer to void

"xcsynt/src/ambig.c", line 1347: warning: argument #7 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxY.h", line 33
	argument : pointer to void
"xcsynt/src/ambig.c", line 1385: warning: argument #2 is incompatible with prototype:
	prototype: pointer to function(pointer to long, pointer to long, long) returning _Bool : "incl/XxY.h", line 132
	argument : pointer to void
"xcsynt/src/ambig.c", line 1467: warning: assignment type mismatch:
	pointer to function(long) returning _Bool "=" pointer to void

"xcsynt/src/ARC.c", line 171: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxYxZ.h", line 35
	argument : pointer to void
"xcsynt/src/ARC.c", line 514: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XH.h", line 46
	argument : pointer to void
"xcsynt/src/ARC.c", line 2682: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XH.h", line 46
	argument : pointer to void
"xcsynt/src/ARC.c", line 2692: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XH.h", line 46
	argument : pointer to void
"xcsynt/src/ARC.c", line 2794: warning: argument #5 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/X.h", line 36
	argument : pointer to void
"xcsynt/src/ARC.c", line 2798: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxYxZ.h", line 35
	argument : pointer to void
"xcsynt/src/ARC.c", line 2802: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxYxZ.h", line 35
	argument : pointer to void

"xcsynt/src/XARC.c", line 157: operands have incompatible types:
	 pointer to function(long, long) returning void ":" pointer to void
"xcsynt/src/XARC.c", line 158: warning: argument #6 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XH.h", line 46
	argument : pointer to void
"xcsynt/src/XARC.c", line 168: warning: argument #7 is incompatible with prototype:
	prototype: pointer to function(long, long) returning void : "incl/XxY.h", line 33
	argument : pointer to void
"xcsynt/src/XARC.c", line 1828: warning: argument #4 is incompatible with prototype:
	prototype: pointer to function(pointer to struct ARC_struct {struct  {..} Q_hd, pointer to unsigned long Q_set, struct  {..} QxTxQ_hd, pointer to long Q_stack, pointer to struct ARC_ATTR {..} attr, pointer to struct wl {..} wl, pointer to struct whd {..} whd, pointer to unsigned long t_set, pointer to unsigned long conflict_t_set, pointer to struct ARC_struct {..} ARC, long pss_kind, long h_value, long k_value, long kind, long conflict_kind, _Bool is_allocated, _Bool is_initiated, _Bool is_XARC, _Bool is_clonable, _Bool is_ARP}, long) returning _Bool : "xcsynt/incl/csynt_optim.h", line 440
	argument : pointer to void
"xcsynt/src/XARC.c", line 1833: warning: argument #4 is incompatible with prototype:
	prototype: pointer to function(pointer to struct ARC_struct {struct  {..} Q_hd, pointer to unsigned long Q_set, struct  {..} QxTxQ_hd, pointer to long Q_stack, pointer to struct ARC_ATTR {..} attr, pointer to struct wl {..} wl, pointer to struct whd {..} whd, pointer to unsigned long t_set, pointer to unsigned long conflict_t_set, pointer to struct ARC_struct {..} ARC, long pss_kind, long h_value, long k_value, long kind, long conflict_kind, _Bool is_allocated, _Bool is_initiated, _Bool is_XARC, _Bool is_clonable, _Bool is_ARP}, long) returning _Bool : "xcsynt/incl/csynt_optim.h", line 440
	argument : pointer to void
cc: acomp failed for xcsynt/src/XARC.c

