/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       ShortMem (1.1)                              *
 * Version:                 1.0                                         *
 * Date:                    February 18, 2000                           *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module is portable.                    *
 *                                                                      *
 * Author:                  Zoran Budimac                               *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_ShortMem_cpp_
#define _M2_ShortMem_cpp_

#include <string.h>

#include <_m2sys.h>

#include <ShortMem.m2h>

namespace ShortMem {

   void Copy(ADDRESS dest, ADDRESS source, CARDINAL n) {
      memcpy((void *)dest, (void *)source, n);
   } // Copy

   template <class _BYTE1>
   void Fill(ADDRESS dest, CARDINAL n, _BYTE1 val) {
      _SINGLEOF<BYTE> _val(_DATAADR(val));
      memset((void *)dest, BYTE(_val), n);
   } // Fill

   BOOLEAN Equal(ADDRESS m1, ADDRESS m2, CARDINAL n) {
      return ((memcmp((void *)m1, (void *)m2, n) == 0) ? TRUE : FALSE);
   } // Equal

   BOOLEAN LEq(ADDRESS m1, ADDRESS m2, CARDINAL n) {
      return ((memcmp((void *)m1, (void *)m2, n) <= 0) ? TRUE : FALSE);
   } // LEq

   BOOLEAN GEq(ADDRESS m1, ADDRESS m2, CARDINAL n) {
      return ((memcmp((void *)m1, (void *)m2, n) >= 0) ? TRUE : FALSE);
   } // GEq

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // ShortMem

#endif
