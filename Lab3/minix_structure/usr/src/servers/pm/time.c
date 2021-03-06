/* This file takes care of those system calls that deal with time.
 *
 * The entry points into this file are
 *   do_time:		perform the TIME system call
 *   do_stime:		perform the STIME system call
 *   do_times:		perform the TIMES system call
 */

#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include "mproc.h"
#include "param.h"

/*===========================================================================*
 *				do_time					     *
 *===========================================================================*/
int do_time()
{
/* Perform the time(tp) system call. This returns the time in seconds since
 * 1.1.1970.  MINIX is an astrophysically naive system that assumes the earth
 * rotates at a constant rate and that such things as leap seconds do not
 * exist.
 */
  clock_t uptime, boottime;
  int s;

  if ( (s=getuptime2(&uptime, &boottime)) != OK)
  	panic("do_time couldn't get uptime: %d", s);

  mp->mp_reply.reply_time = (time_t) (boottime + (uptime/system_hz));
  mp->mp_reply.reply_utime = (uptime%system_hz)*1000000/system_hz;
  return(OK);
}

/*===========================================================================*
 *        do_utctime          * ADDED FOR OS-EXERCISE *
 *===========================================================================*/
int do_utctime()
{
/* Perform the utctime(tp) system call. This returns the time in seconds since
 * 1.1.1970, while taking leap seconds into account (until and excluding June 30 2018)
 * Since time_t is a 32-bit integer in this version of Minix, we will run into integer
 * overflow issues around the year 2038.
 */
  clock_t uptime, boottime;
  time_t utctime;
  int s;
  if ( (s=getuptime2(&uptime, &boottime)) != OK)
    panic("do_utctime couldn get uptime: %d", s);

  utctime = (boottime + (uptime/system_hz));

  //hardcoded timestamps after which a leap second was inserted
  time_t leapsecond_reference[27] = {78796799, 94694399, 126230399, 157766399, \
    189302399, 220924799, 252460799, 283996799, 315532799, 362793599, 394329599, \
    425865599, 489023999, 567993599, 631151999, 662687999, 709948799, 741484799, \
    773020799, 820454399, 867715199, 915148799, 1136073599, 1230767999, 1341100799, \
    1435708799, 1483228799};
  int i;
  for(i = 0; i < 27; ++i)
    if(!(utctime > leapsecond_reference[i]))
      break;

  mp->mp_reply.reply_time = utctime + i + 10; //add 10 inherent leap seconds.
  mp->mp_reply.reply_utime = (uptime%system_hz)*1000000/system_hz;

  return(OK);
}


/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
int do_stime()
{
/* Perform the stime(tp) system call. Retrieve the system's uptime (ticks
 * since boot) and pass the new time in seconds at system boot to the kernel.
 */
  clock_t uptime, boottime;
  int s;

  if (mp->mp_effuid != SUPER_USER) {
      return(EPERM);
  }
  if ( (s=getuptime(&uptime)) != OK)
      panic("do_stime couldn't get uptime: %d", s);
  boottime = (long) m_in.stime - (uptime/system_hz);

  s= sys_stime(boottime);		/* Tell kernel about boottime */
  if (s != OK)
	panic("pm: sys_stime failed: %d", s);

  return(OK);
}

/*===========================================================================*
 *				do_times				     *
 *===========================================================================*/
int do_times()
{
/* Perform the times(buffer) system call. */
  register struct mproc *rmp = mp;
  clock_t user_time, sys_time, uptime;
  int s;

  if (OK != (s=sys_times(who_e, &user_time, &sys_time, &uptime, NULL)))
      panic("do_times couldn't get times: %d", s);
  rmp->mp_reply.reply_t1 = user_time;		/* user time */
  rmp->mp_reply.reply_t2 = sys_time;		/* system time */
  rmp->mp_reply.reply_t3 = rmp->mp_child_utime;	/* child user time */
  rmp->mp_reply.reply_t4 = rmp->mp_child_stime;	/* child system time */
  rmp->mp_reply.reply_t5 = uptime;		/* uptime since boot */

  return(OK);
}
