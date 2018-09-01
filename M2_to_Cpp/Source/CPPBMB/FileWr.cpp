/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       FileWr (1.1)                                *
 * Version:                 1.0                                         *
 * Date:                    June 5, 2000                                *
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

#ifndef _M2_FileWr_cpp_
#define _M2_FileWr_cpp_

#include <_m2sys.h>

#include <FileWr.m2h>

#include <File.m2h>
#include <Fmt.m2h>
#include <File.cpp>
#include <Fmt.cpp>

namespace FileWr {

   typedef ARRAY<_SubrangeType<CARDINAL, 0, 260>, CHAR> St;

   void Bool(File::T f, BOOLEAN b, CARDINAL field) {
      St s;
      Fmt::Bool(s, b);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // Bool

   void Int(File::T f, INTEGER n, CARDINAL field) {
      St s;
      Fmt::Int(s, n);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // Int

   void LongInt(File::T f, LONGINT n, CARDINAL field) {
      St s;
      Fmt::LongInt(s, n);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // LongInt

   void Card(File::T f, CARDINAL n, CARDINAL field) {
      St s;
      Fmt::Card(s, n);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // Card

   void LongCard(File::T f, LONGCARD n, CARDINAL field) {
      St s;
      Fmt::LongCard(s, n);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // LongCard

   void Real(File::T f, REAL n, CARDINAL field) {
      St s;
      Fmt::Real(s, n);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // Real

   void LongReal(File::T f, LONGREAL n, CARDINAL field) {
      St s;
      Fmt::LongReal(s, n);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // LongReal

   void IntB(File::T f, INTEGER n, CARDINAL base, CARDINAL field) {
      St s;
      Fmt::IntB(s, n, base);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // IntB

   void LongIntB(File::T f, LONGINT n, CARDINAL base, CARDINAL field) {
      St s;
      Fmt::LongIntB(s, n, base);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // LongIntB

   void CardB(File::T f, CARDINAL n, CARDINAL base, CARDINAL field) {
      St s;
      Fmt::CardB(s, n, base);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // CardB

   void LongCardB(File::T f, LONGCARD n, CARDINAL base, CARDINAL field) {
      St s;
      Fmt::LongCardB(s, n, base);
      if (field != 0) Fmt::PadR(s, s, field, ' ');
      File::WrStr(f, s);
   } // LongCardB

   template <class _BYTEARRAY1>
   void Bin(File::T f, _BYTEARRAY1 bin) {
      _ARRAYOF<BYTE> _bin(_DATAADR(bin), _DATASIZE(bin));

      File::WrBin(f, bin, LONGCARD(_DATASIZE(_bin)));
   } // Bin

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // FileWr

#endif
