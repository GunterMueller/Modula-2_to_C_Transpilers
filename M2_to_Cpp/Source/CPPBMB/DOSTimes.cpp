/* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'Time' module. It contains operations for getting
    and setting the system time. Operations are characteristic for the
    DOS platform and rely on the non standard, but common dos.h

    by Milos Radovanovic, June 8, 2000
    ------------------------------------------------------------------ */

#ifndef _M2_DOSTimes_cpp_
#define _M2_DOSTimes_cpp_

#include <dos.h>

#include <_m2sys.h>

#include <DOSTimes.m2h>

#include <Time.m2h>
#include <Time.cpp>

namespace DOSTimes {

   void SetST(Time::T t, BOOLEAN& ok) {
      dostime_t dt;

      dt.hour    = t.hour;
      dt.minute  = t.min;
      dt.second  = t.sec;
      dt.hsecond = t.hun;

      _dos_settime(&dt);
      ok = TRUE;
   } // SetST

   void GetST(Time::T& t, BOOLEAN& ok) {
      dostime_t dt;

      _dos_gettime(&dt);

      t.hour = CARDINAL(dt.hour);
      t.min  = CARDINAL(dt.minute);
      t.sec  = CARDINAL(dt.second) ;
      t.hun  = CARDINAL(dt.hsecond);

      ok = TRUE;
   } // GetST

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // DOSTimes

#endif
