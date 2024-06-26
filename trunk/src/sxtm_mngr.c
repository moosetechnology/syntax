/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

/* Gestion du temps et horodatage */

#ifdef WINNT
#define _WIN32_WINNT 0x0500  /* voodoo magic */
#include <windows.h>
#undef ERROR                 /* these 2 undefs are needed to avoid name */
#undef void                  /* conflicts between windows.h and sxunix.h */

HANDLE hTimerQueue = NULL;
static int nb_running_timers = 0; /* this static variable counts the number of */
                                  /* timers currently running */
#endif /* ifndef WINNT */

#include "sxversion.h"
#include "sxunix.h"


char WHAT_SXTM_MNGR[] = "@(#)SYNTAX - $Id: sxtm_mngr.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

#ifndef WINNT
#include <sys/resource.h>
#endif /* ifndef WINNT */

#include <signal.h>
#include <sys/time.h>

SXINT
sxtimediff (SXINT INIT_ou_ACTION)
{
  SXINT diff;

#ifndef WINNT
  {
    struct rusage fin;

    getrusage (RUSAGE_SELF, &fin);
    if (INIT_ou_ACTION == SXINIT)
      diff = 0;
    else
      diff = (SXINT) 1000 * (fin.ru_utime.tv_sec - sxtime_v.tv_sec) + (fin.ru_utime.tv_usec - sxtime_v.tv_usec) / (SXINT) 1000;
    sxtime_v = fin.ru_utime;
  }
#else /* WINNT */
  {
    clock_t fin;

    fin = clock ();
    if (INIT_ou_ACTION == SXINIT)
      diff = 0;
    else
      diff = (fin - sxtime_v);
    sxtime_v = fin; 
  }
#endif /* WINNT */
  return diff;
}


void
sxtimestamp (SXINT INIT_ou_ACTION, char *format)
{
  if (INIT_ou_ACTION == SXINIT)
    sxtimediff (SXINIT);
  else
    fprintf (sxtty, format != NULL ? format : " (%ldms)\n", sxtimediff (SXACTION));
}


void
sxtime (SXINT INIT_ou_ACTION, char *str)
{
  if (INIT_ou_ACTION == SXINIT) {
    sxtimestamp (SXINIT, NULL);

    if (str)
      fputs (str, sxtty);
  }
  else {
    fputs (str, sxtty);
    sxtimestamp (SXACTION, NULL);
  }
}

/* typedef void (*sighandler_t)(SXINT); */

#ifdef WINNT

/* this is a wrapper which calls the actual callback (param #1 lpParam) */
/* and then does some cleanup if all the timers are finished */

void CALLBACK sxtm_callback_wrapper(PVOID lpParam, /* Windows' */ BOOLEAN TimerOrWaitFired)
{
  void (*callback)(void);
  callback =  lpParam;
  sxuse(TimerOrWaitFired);

  callback();

  nb_running_timers --;

  if ( (nb_running_timers == 0)  && (hTimerQueue != NULL) ) {
    DeleteTimerQueue ( hTimerQueue );
    hTimerQueue = NULL;
  }
}

#endif

void
sxexit_timeout (int signal_val)
{
  sxuse(signal_val); /* pour faire taire gcc -Wunused */
  sxexit (4);
}

void
sxcaught_timeout (int delay, void (*sxtimeout_handler) (int))
{
  if (sxtimeout_handler) {
#ifndef WINNT
    signal (SIGALRM, sxtimeout_handler);
    alarm ((unsigned int)delay);
    /* sxtimeout_handler est appele' au bout de delay seconds */
#else
    /* WINNT case */
    HANDLE hTimer = NULL;
    DWORD delay_ms = delay*1000;

    nb_running_timers ++;

    if ( hTimerQueue == NULL ) {
      if ( ( hTimerQueue = CreateTimerQueue() ) == NULL ) {
	printf("CreateTimerQueue failed\n");
	sxexit (1);
      }
    }

    /* the callback provided by the caller is param #2 of sxcaught_timeout(): 'sxtimeout_handler' */
    /* it is passed along to our wrapper 'sxtm_callback_wrapper', as param #4 of CreateTimerQueueTimer() */ 

    if (!CreateTimerQueueTimer ( &hTimer,
				 hTimerQueue,
				 (WAITORTIMERCALLBACK)sxtm_callback_wrapper,
				 sxtimeout_handler,
				 delay_ms,0,0)) {
      printf("CreateTimerQueueTimer failed\n");
      sxexit (1);
    }
#endif
  }
}

void
sxtimeout (int delay)
{
  sxcaught_timeout (delay, sxexit_timeout);
  /* sxexit_timeout est appele' au bout de delay seconds */
}

/*
  SXINT getitimer(SXINT which, struct itimerval *value);
  SXINT  setitimer(SXINT which, const struct itimerval *value, struct itimer-val *ovalue);
  ITIMER_VIRTUAL decrements  only  when  the  process  is  executing, and delivers SIGVTALRM upon expiration.
*/


/* sxvirtual_timeout_handler est appele' au bout de seconds secondes et useconds microsecondes */
void
sxcaught_virtual_timeout (SXINT seconds, SXINT useconds, void (*sxvirtual_timeout_handler) (int))
{
  if (sxvirtual_timeout_handler) {
#ifndef WINNT
    struct itimerval virtual_time;

    signal (SIGVTALRM, sxvirtual_timeout_handler);
    virtual_time.it_value.tv_sec = seconds;
    virtual_time.it_value.tv_usec = useconds;
    virtual_time.it_interval.tv_sec = virtual_time.it_interval.tv_usec = 0;
    setitimer (ITIMER_VIRTUAL, &virtual_time, (struct itimerval*) NULL);
#else
    /* ITIMER_VIRTUAL and setitimer() are not available for the win32 architecture */
    /* we choose to wait with a normal timer, which counts real time, not virtual time */
    sxcaught_timeout( 2*seconds + 0*useconds, sxvirtual_timeout_handler );
#endif
  }
}

void
sxvirtual_timeout (SXINT seconds, SXINT useconds)
{
  sxcaught_virtual_timeout (seconds, useconds, sxexit_timeout);
}

