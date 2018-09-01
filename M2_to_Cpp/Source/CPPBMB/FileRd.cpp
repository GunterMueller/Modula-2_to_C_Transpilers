/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       FileRd (1.1)                                *
 * Version:                 1.0                                         *
 * Date:                    June 6, 2000                                *
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

#ifndef _M2_FileRd_cpp_
#define _M2_FileRd_cpp_

#include <_m2sys.h>

#include <FileRd.m2h>

#include <File.m2h>
#include <Scan.m2h>
#include <String.m2h>
#include <File.cpp>
#include <Scan.cpp>
#include <String.cpp>

namespace FileRd {

   typedef ARRAY<_SubrangeType<CARDINAL, 0, 260>, CHAR> St;

   void SkipSpaces(File::T f, CHAR& ch) {
      /* Skips spaces in file 'f', returning the last non-space character
         in 'ch' */
      ch = File::RdChar(f);
      while ((ch == ' ') || (ch == '\011') || (ch == '\015') || (ch == '\012'))
         ch = File::RdChar(f);
   } // SkipSpaces

   BOOLEAN Bool(File::T f) {
      /* by Zoran Budimac, December, 1996 */
      St s;
      BOOLEAN b;
      CHAR ch;

      SkipSpaces(f, ch);
      String::Copy(s, "");
      if (ch == 'T') {
         String::Append(s, ch);
         ch = File::RdChar(f);
         if (ch == 'R') {
            String::Append(s, ch);
            ch = File::RdChar(f);
            if (ch == 'U') {
               String::Append(s, ch);
               ch = File::RdChar(f);
               if (ch == 'E') {
                  String::Append(s, ch);
                  ch = File::RdChar(f);
               }
            }
         }
      }
      else if (ch == 'F') {
         String::Append(s, ch);
         ch = File::RdChar(f);
         if (ch == 'A') {
            String::Append(s, ch);
            ch = File::RdChar(f);
            if (ch == 'L') {
               String::Append(s, ch);
               ch = File::RdChar(f);
               if (ch == 'S') {
                  String::Append(s, ch);
                  ch = File::RdChar(f);
                  if (ch == 'E') {
                     String::Append(s, ch);
                     ch = File::RdChar(f);
                  }
               }
            }
         }
      }
      b = Scan::Bool(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return b;
   } // Bool

   INTEGER Int(File::T f) {
      /* by Zoran Budimac, December, 1996 */
      CHAR ch;
      St s;
      INTEGER n;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      if (ch == '-') {
         String::Append(s, '-');
         ch = File::RdChar(f);
      }
      while ((ch >= '0') && (ch <= '9')) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::Int(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // Int

   LONGINT LongInt(File::T f) {
      /* by Zoran Budimac, December, 1996 */
      CHAR ch;
      St s;
      LONGINT n;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      if (ch == '-') {
         String::Append(s, '-');
         ch = File::RdChar(f);
      }
      while ((ch >= '0') && (ch <= '9')) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::LongInt(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // LongInt

   CARDINAL Card(File::T f) {
      /* by Zoran Budimac, December 1996 */
      St s;
      CARDINAL n;
      CHAR ch;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      while ((ch >= '0') && (ch <= '9')) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::Card(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // Card

   LONGCARD LongCard(File::T f) {
      /* by Zoran Budimac, December 1996 */
      St s;
      LONGCARD n;
      CHAR ch;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      while ((ch >= '0') && (ch <= '9')) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::LongCard(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // LongCard

   REAL Real(File::T f) {
      /* by Zoran Budimac, December 1996 */
      St s;
      REAL n;
      CHAR ch;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      if (ch == '-') {
         String::Append(s, '-');
         ch = File::RdChar(f);
      }
      while ((ch >= '0') && (ch <= '9')) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      if (ch == '.') {
         String::Append(s, '.');
         ch = File::RdChar(f);
         while ((ch >= '0') && (ch <= '9')) {
            String::Append(s, ch);
            ch = File::RdChar(f);
         }
      }
      if (CAP(ch) == 'E') {
         String::Append(s, 'E');
         ch = File::RdChar(f);
         if (ch == '+') {
            String::Append(s, '+');
            ch = File::RdChar(f);
         }
         else if (ch == '-') {
            String::Append(s, '-');
            ch = File::RdChar(f);
         }
         while ((ch >= '0') && (ch <= '9')) {
            String::Append(s, ch);
            ch = File::RdChar(f);
         }
      }
      n = Scan::Real(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // Real

   LONGREAL LongReal(File::T f) {
      /* by Zoran Budimac, December 1996 */
      St s;
      LONGREAL n;
      CHAR ch;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      if (ch == '-') {
         String::Append(s, '-');
         ch = File::RdChar(f);
      }
      while ((ch >= '0') && (ch <= '9')) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      if (ch == '.') {
         String::Append(s, '.');
         ch = File::RdChar(f);
         while ((ch >= '0') && (ch <= '9')) {
            String::Append(s, ch);
            ch = File::RdChar(f);
         }
      }
      if (CAP(ch) == 'E') {
         String::Append(s, 'E');
         ch = File::RdChar(f);
         if (ch == '+') {
            String::Append(s, '+');
            ch = File::RdChar(f);
         }
         else if (ch == '-') {
            String::Append(s, '-');
            ch = File::RdChar(f);
         }
         while ((ch >= '0') && (ch <= '9')) {
            String::Append(s, ch);
            ch = File::RdChar(f);
         }
      }
      n = Scan::LongReal(s);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // LongReal

   INTEGER IntB(File::T f, Scan::Base base) {
      /* by Zoran Budimac, December, 1996 */
      CHAR ch;
      St s;
      INTEGER n;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      if (ch == '-') {
         String::Append(s, '-');
         ch = File::RdChar(f);
      }
      while (((ch >= '0') && (ch <= '9')) ||
             ((CAP(ch) >= 'A') && (CAP(ch) <= 'F'))) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::IntB(s, base);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // IntB

   LONGINT LongIntB(File::T f, Scan::Base base) {
      /* by Zoran Budimac, December, 1996 */
      CHAR ch;
      St s;
      LONGINT n;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      if (ch == '-') {
         String::Append(s, '-');
         ch = File::RdChar(f);
      }
      while (((ch >= '0') && (ch <= '9')) ||
             ((CAP(ch) >= 'A') && (CAP(ch) <= 'F'))) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::LongIntB(s, base);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // LongIntB

   CARDINAL CardB(File::T f, Scan::Base base) {
      /* by Zoran Budimac, December, 1996 */
      CHAR ch;
      St s;
      CARDINAL n;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      while (((ch >= '0') && (ch <= '9')) ||
             ((CAP(ch) >= 'A') && (CAP(ch) <= 'F'))) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::CardB(s, base);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // CardB

   LONGCARD LongCardB(File::T f, Scan::Base base) {
      /* by Zoran Budimac, December, 1996 */
      CHAR ch;
      St s;
      LONGCARD n;

      String::Copy(s, "");
      SkipSpaces(f, ch);
      while (((ch >= '0') && (ch <= '9')) ||
             ((CAP(ch) >= 'A') && (CAP(ch) <= 'F'))) {
         String::Append(s, ch);
         ch = File::RdChar(f);
      }
      n = Scan::LongCardB(s, base);
      /*if (!File::IsInteractive(f))*/ File::UnRdChar(f);
      return n;
   } // LongCardB

   template <class BYTEARRAY1>
   void Bin(File::T f, BYTEARRAY1& bin) {
      _ARRAYOF<BYTE> _bin(_DATAADR(bin), _DATASIZE(bin));

      LONGCARD dummy = File::RdBin(f, _bin, LONGCARD(_DATASIZE(_bin)));
   } // Bin

   void _ModuleBody::BEGIN() {
   }

   void _ModuleBody::FINALLY() {
   }

} // FileRd

#endif
