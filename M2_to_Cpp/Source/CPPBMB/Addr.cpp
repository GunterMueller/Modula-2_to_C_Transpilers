/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Addr (1.1)                                  *
 * Version:                 1.0                                         *
 * Date:                    February 18, 2000                           *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module is portable.                    *
 *                                                                      *
 * Author:                  Daniel Sendula                              *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Addr_cpp_
#define _M2_Addr_cpp_

#include <_m2sys.h>

#include <Addr.m2h>

namespace Addr {

   const CARDINAL MAXCARD = MAX(CARDINAL);

   void Inc(ADDRESS& a, LONGCARD n) {
      while (n > MAXCARD) {
         a = ADDADR(a, MAXCARD);
      }
      a = ADDADR(a, CARDINAL(n));
   } // Inc

   void Dec(ADDRESS& a, LONGCARD n) {
      while (n > MAXCARD) {
         a = SUBADR(a, MAXCARD);
      }
      a = SUBADR(a, CARDINAL(n));
   } // Dec

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // Addr

#endif
