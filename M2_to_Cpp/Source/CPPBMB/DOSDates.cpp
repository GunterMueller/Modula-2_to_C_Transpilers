/* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'Date' module. It contains operations for getting
    and setting the system date. Operations are characteristic for the
    DOS platform and rely on the non standard, but common dos.h

    by Milos Radovanovic, June 8, 2000
    ------------------------------------------------------------------ */

#ifndef _M2_DOSDates_cpp_
#define _M2_DOSDates_cpp_

#include <dos.h>

#include <_m2sys.h>

#include <DOSDates.m2h>

#include <Date.m2h>
#include <Date.cpp>

namespace DOSDates {

   void SetSD(Date::T d, BOOLEAN& ok) {
      dosdate_t dd;

      dd.day       = d.day;
      dd.month     = d.month;
      dd.year      = d.year;
      dd.dayofweek = Date::DayOfWeek(d);

      _dos_setdate(&dd);
      ok = TRUE;
   } // SetSD

   void GetSD(Date::T& d, BOOLEAN& ok) {
      dosdate_t dd;

      _dos_getdate(&dd);

      d.day   = CARDINAL(dd.day);
      d.month = CARDINAL(dd.month);
      d.year  = CARDINAL(dd.year) ;

      ok = TRUE;
   } // GetSD

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // DOSDates

#endif
