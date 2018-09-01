/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Wr (1.1)                                    *
 * Version:                 1.0                                         *
 * Date:                    June 5, 2000                                *
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

#ifndef _M2_Wr_cpp_
#define _M2_Wr_cpp_

#include <_m2sys.h>

#include <Wr.m2h>

#include <File.m2h>
#include <FileWr.m2h>
#include <File.cpp>
#include <FileWr.cpp>

namespace Wr {

   void Bool(BOOLEAN b, CARDINAL field) {
      FileWr::Bool(File::StdOut(), b, field);
      File::Flush(File::StdOut());
   } // Bool

   void Int(INTEGER n, CARDINAL field) {
      FileWr::Int(File::StdOut(), n, field);
      File::Flush(File::StdOut());
   } // Int

   void LongInt(LONGINT n, CARDINAL field) {
      FileWr::LongInt(File::StdOut(), n, field);
      File::Flush(File::StdOut());
   } // LongInt

   void Card(CARDINAL n, CARDINAL field) {
      FileWr::Card(File::StdOut(), n, field);
      File::Flush(File::StdOut());
   } // Card

   void LongCard(LONGCARD n, CARDINAL field) {
      FileWr::LongCard(File::StdOut(), n, field);
      File::Flush(File::StdOut());
   } // LongCard

   void Real(REAL n, CARDINAL field) {
      FileWr::Real(File::StdOut(), n, field);
      File::Flush(File::StdOut());
   } // Real

   void LongReal(LONGREAL n, CARDINAL field) {
      FileWr::LongReal(File::StdOut(), n, field);
      File::Flush(File::StdOut());
   } // LongReal

   void IntB(INTEGER n, CARDINAL base, CARDINAL field) {
      FileWr::IntB(File::StdOut(), n, base, field);
      File::Flush(File::StdOut());
   } // IntB

   void LongIntB(LONGINT n, CARDINAL base, CARDINAL field) {
      FileWr::LongIntB(File::StdOut(), n, base, field);
      File::Flush(File::StdOut());
   } // LongIntB

   void CardB(CARDINAL n, CARDINAL base, CARDINAL field) {
      FileWr::CardB(File::StdOut(), n, base, field);
      File::Flush(File::StdOut());
   } // CardB

   void LongCardB(LONGCARD n, CARDINAL base, CARDINAL field) {
      FileWr::LongCardB(File::StdOut(), n, base, field);
      File::Flush(File::StdOut());
   } // LongCardB

   void Char(CHAR ch) {
      FileWr::Char(File::StdOut(), ch);
      File::Flush(File::StdOut());
   } // Char

   void Str(_ARRAYOF<CHAR> s) {
      FileWr::Str(File::StdOut(), s);
      File::Flush(File::StdOut());
   } // Str

   void Ln() {
      FileWr::Ln(File::StdOut());
      File::Flush(File::StdOut());
   } // Ln

   template <class _BYTEARRAY1>
   void Bin(_BYTEARRAY1 bin) {
      _ARRAYOF<BYTE> _bin(_DATAADR(bin), _DATASIZE(bin));
      FileWr::Bin(File::StdOut(), _bin);
      File::Flush(File::StdOut());
   } // Bin

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // Wr

#endif
