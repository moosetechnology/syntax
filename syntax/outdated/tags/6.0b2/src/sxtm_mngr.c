/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2003 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'équipe ATOLL.  (phd)                   *
   *                                                      *
   ******************************************************** */




/* Gestion du temps et horodatage */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20060426 15:51 (bs):	        Possibilite de capturer les timeout   	*/
/************************************************************************/
/* 20041220 13:51 (pb):	        Ajout de sxtime et sxtimeout   		*/
/************************************************************************/
/* 20040213 14:51 (phd):	#if !WINNT au lieu de #if unix		*/
/************************************************************************/
/* 20030318 10:39 (phd) :	Création				*/
/************************************************************************/

#ifdef WINNT
#define _WIN32_WINNT 0x0500  /* voodoo magic */
#include <windows.h>
#undef ERROR                 /* these 2 undefs are needed to avoid name */
#undef VOID                  /* conflicts between windows.h and sxunix.h */

HANDLE hTimerQueue = NULL;
static int nb_running_timers = 0; /* this static variable counts the number of */
                                  /* timers currently running */

#endif /* ifndef WINNT */

#include	"sxunix.h"


char WHAT_SXTM_MNGR[] = "@(#)SYNTAX - $Id: sxtm_mngr.c 652 2007-06-22 09:09:59Z rlacroix $" WHAT_DEBUG;

#ifndef WINNT
# include <sys/resource.h>
#endif /* ifndef WINNT */

long
sxtimediff (int INIT_ou_ACTION)
{
  long diff;

#ifndef WINNT
  {
    struct rusage fin;

    getrusage (RUSAGE_SELF, &fin);
    if (INIT_ou_ACTION == INIT)
      diff = 0L;
    else
      diff = 1000L * (fin.ru_utime.tv_sec - sxtime_v.tv_sec) + (fin.ru_utime.tv_usec - sxtime_v.tv_usec) / 1000L;
    sxtime_v = fin.ru_utime;
  }
#else /* WINNT */
  {
    clock_t fin;

    fin = clock ();
    if (INIT_ou_ACTION == INIT)
      diff = 0L;
    else
      diff = (fin - sxtime_v);
    sxtime_v = fin; 
  }
#endif /* WINNT */
  return diff;
}


VOID
sxtimestamp (int INIT_ou_ACTION, char *format)
{
  if (INIT_ou_ACTION == INIT)
    sxtimediff (INIT);
  else
    fprintf (sxtty, format != NULL ? format : " (%ldms)\n", sxtimediff (ACTION));
}


VOID
sxtime (int INIT_ou_ACTION, char *str)
{
  if (INIT_ou_ACTION == INIT) {
    sxtimestamp (INIT, NULL);

    if (str)
      fputs (str, sxtty);
  }
  else {
    fputs (str, sxtty);
    sxtimestamp (ACTION, NULL);
  }
}


#include <signal.h>

/* typedef void (*sighandler_t)(int); */

#ifdef WINNT

/* this is a wrapper which calls the actual callback (param #1 lpParam) */
/* and then does some cleanup if all the timers are finished */

VOID CALLBACK sxtm_callback_wrapper(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
  VOID (*callback)();
  callback =  lpParam;

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
  SXEXIT (4);
}

VOID
sxcaught_timeout (delay, sxtimeout_handler)
     int delay;
     VOID (*sxtimeout_handler) ();
{
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
      SXEXIT(1);
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
    SXEXIT(1);
  }
#endif
}

VOID
sxtimeout (int delay)
{
  sxcaught_timeout (delay, sxexit_timeout);
  /* sxexit_timeout est appele' au bout de delay seconds */
}

#include <sys/time.h>
/*
  int getitimer(int which, struct itimerval *value);
  int  setitimer(int which, const struct itimerval *value, struct itimer-val *ovalue);
  ITIMER_VIRTUAL decrements  only  when  the  process  is  executing, and delivers SIGVTALRM upon expiration.
*/


/* sxvirtual_timeout_handler est appele' au bout de seconds secondes et useconds microsecondes */
VOID
sxcaught_virtual_timeout (seconds, useconds, sxvirtual_timeout_handler)
     long seconds, useconds;
     VOID (*sxvirtual_timeout_handler) ();
{
#ifndef WINNT
  struct itimerval virtual_time;

  signal (SIGVTALRM, sxvirtual_timeout_handler);
  virtual_time.it_value.tv_sec = seconds;
  virtual_time.it_value.tv_usec = useconds;
  virtual_time.it_interval.tv_sec = virtual_time.it_interval.tv_usec = 0L;
  setitimer (ITIMER_VIRTUAL, &virtual_time, (struct itimerval*)NULL);
#else
  /* ITIMER_VIRTUAL and setitimer() are not available for the win32 architecture */
  /* we choose to wait with a normal timer, which counts real time, not virtual time */
  sxcaught_timeout( 2*seconds, sxvirtual_timeout_handler );
#endif
}

VOID
sxvirtual_timeout (seconds, useconds)
     long seconds, useconds;
{
  sxcaught_virtual_timeout (seconds, useconds, sxexit_timeout);
}

