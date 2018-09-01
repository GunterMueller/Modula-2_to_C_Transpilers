/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                             C++ Port                                 *
 *                                                                      *
 * Implementation of:       Mem (1.1)                                   *
 * Version:                 1.0                                         *
 * Date:                    February 18, 2000                           *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module is portable.                    *
 *                                                                      *
 * Author:                  Daniel Sendula                              *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: isendula@unsim.ns.ac.yu         *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Mem_cpp_
#define _M2_Mem_cpp_

#include <_m2sys.h>

#include <Mem.m2h>

#include <ShortMem.m2h>
#include <Addr.m2h>
#include <ShortMem.cpp>
#include <Addr.cpp>

namespace Mem {

   const CARDINAL MAXCARD = MAX(CARDINAL);

   void Copy(ADDRESS dest, ADDRESS source, LONGCARD n) {
      while (n > MAXCARD) {
         ShortMem::Copy(dest, source, MAXCARD);
         DEC(n, MAXCARD);
         Addr::Inc(dest, MAXCARD);
         Addr::Inc(source, MAXCARD);
      }
      if (n > 0) {
         ShortMem::Copy(dest, source, CARDINAL(n));
      }
   } // Copy

   template <class _BYTE1>
   void Fill(ADDRESS dest, LONGCARD n, _BYTE1 val) {

      _SINGLEOF<BYTE> _val(_DATAADR(val));

      while (n > MAXCARD) {
         ShortMem::Fill(dest, MAXCARD, BYTE(_val));
         DEC(n, MAXCARD);
         Addr::Inc(dest, MAXCARD);
      }
      if (n > 0) {
         ShortMem::Fill(dest, CARDINAL(n), BYTE(_val));
      }
   } // Fill

   BOOLEAN Equal(ADDRESS m1, ADDRESS m2, LONGCARD n) {

      BOOLEAN stillEqual = TRUE;

      while (stillEqual && (n > MAXCARD)) {
         stillEqual = ShortMem::Equal(m1, m2, MAXCARD);
         Addr::Inc(m1, MAXCARD);
         Addr::Inc(m2, MAXCARD);
         DEC(n, MAXCARD);
      }
      if (stillEqual && (n > 0)) {
         stillEqual = ShortMem::Equal(m1, m2, CARDINAL(n));
      }
      return stillEqual;
   } // Equal

   BOOLEAN LEq(ADDRESS m1, ADDRESS m2, LONGCARD n) {

      BOOLEAN stillLEq = TRUE;

      while (stillLEq && (n > MAXCARD)) {
         stillLEq = ShortMem::LEq(m1, m2, MAXCARD);
         Addr::Inc(m1, MAXCARD);
         Addr::Inc(m2, MAXCARD);
         DEC(n, MAXCARD);
      }
      if (stillLEq && (n > 0)) {
         stillLEq = ShortMem::LEq(m1, m2, CARDINAL(n));
      }
      return stillLEq;
   } // LEq

   BOOLEAN GEq(ADDRESS m1, ADDRESS m2, LONGCARD n) {

      BOOLEAN stillGEq = TRUE;

      while (stillGEq && (n > MAXCARD)) {
         stillGEq = ShortMem::GEq(m1, m2, MAXCARD);
         Addr::Inc(m1, MAXCARD);
         Addr::Inc(m2, MAXCARD);
         DEC(n, MAXCARD);
      }
      if (stillGEq && (n > 0)) {
         stillGEq = ShortMem::GEq(m1, m2, CARDINAL(n));
      }
      return stillGEq;
   } // GEq

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // Mem

#endif
