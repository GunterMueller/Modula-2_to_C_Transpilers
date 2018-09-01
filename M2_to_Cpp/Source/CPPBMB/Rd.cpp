/* -------------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library                   *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Rd (1.1)                                    *
 * Version:                 1.0                                         *
 * Date:                    August 12, 2000                             *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module portable.                       *
 *                                                                      *
 * Author:                  Zoran Budimac                               *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Rd_cpp_
#define _M2_Rd_cpp_

#include <_m2sys.h>

#include <Rd.m2h>

#include <File.m2h>
#include <FileRd.m2h>
#include <File.cpp>
#include <FileRd.cpp>

namespace Rd {

   BOOLEAN Bool() {
      BOOLEAN res = FileRd::Bool(File::StdIn());
      Ln();
      return res;
   } // Bool

   INTEGER Int() {
      INTEGER res = FileRd::Int(File::StdIn());
      Ln();
      return res;
   } // Int

   LONGINT LongInt() {
      LONGINT res = FileRd::LongInt(File::StdIn());
      Ln();
      return res;
   } // LongInt

   CARDINAL Card() {
      CARDINAL res = FileRd::Card(File::StdIn());
      Ln();
      return res;
   } // Card

   LONGCARD LongCard() {
      LONGCARD res = FileRd::LongCard(File::StdIn());
      Ln();
      return res;
   } // LongCard

   REAL Real() {
      REAL res = FileRd::Real(File::StdIn());
      Ln();
      return res;
   } // Real

   LONGREAL LongReal() {
      LONGREAL res = FileRd::LongReal(File::StdIn());
      Ln();
      return res;
   } // LongReal

   INTEGER IntB(CARDINAL base) {
      INTEGER res = FileRd::IntB(File::StdIn(), base);
      Ln();
      return res;
   } // IntB

   LONGINT LongIntB(CARDINAL base) {
      LONGINT res = FileRd::LongIntB(File::StdIn(), base);
      Ln();
      return res;
   } // LongIntB

   CARDINAL CardB(CARDINAL base) {
      CARDINAL res = FileRd::CardB(File::StdIn(), base);
      Ln();
      return res;
   } // CardB

   LONGCARD LongCardB(CARDINAL base) {
      LONGCARD res = FileRd::LongCardB(File::StdIn(), base);
      Ln();
      return res;
   } // LongCardB

   CHAR Char() {
      CHAR res = FileRd::Char(File::StdIn());
      Ln();
      return res;
   } // Char

   void Line(_ARRAYOF<CHAR>& line) {
      FileRd::Line(File::StdIn(), line);
   } // Line

   void Ln() {
      FileRd::Ln(File::StdIn());
   } // Ln

   template <class _BYTEARRAY1>
   void Bin(_BYTEARRAY1& bin) {
      _ARRAYOF<BYTE> _bin(_DATAADR(bin), _DATASIZE(bin));
      FileRd::Bin(File::StdIn(), _bin);
   } // Bin

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // Rd

#endif
