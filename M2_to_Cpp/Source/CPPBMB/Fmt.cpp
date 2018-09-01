/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Fmt (1.10)                                  *
 * Version:                 1.0                                         *
 * Date:                    June 4, 2000                                *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module is portable.                    *
 *                                                                      *
 * Authors:                 Tamas Cservenak & Zoran Budimac             *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: {icervenak,zjb}@unsim.ns.ac.yu  *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Fmt_cpp_
#define _M2_Fmt_cpp_

#include <stdio.h>

#include <_m2sys.h>

#include <Fmt.m2h>

#include <String.m2h>
#include <Wr.m2h>
#include <String.cpp>
#include <Wr.cpp>

namespace Fmt {

   const CARDINAL minBase = 2;
   const CARDINAL maxBase = 16;

   CARDINAL errorCode = noError;

   ARRAY <_SubrangeType<CARDINAL, 0, 15>, CHAR> digitChars = "0123456789ABCDEF";

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** Fmt::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case noRoomError:
            Wr::Str("No room in destination string");
            break;
         case wrongBaseError:
            Wr::Str("Wrong base");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error


   void WholeToStr(LONGCARD num, Base base, _ARRAYOF<CHAR>& str,
                   BOOLEAN& done) {
      /* Convert 'num' to 'str' using 'base' */
      CARDINAL i, j;
      done = TRUE;
      i = HIGH(str)+1;
      do { /* str[i..HIGH(str)] contains conversion result */
         if (i <= 0) {  /* error: num does not fit into str */
            done = FALSE;
            return;
         }
         DEC(i);
         str[i] = digitChars[CARDINAL(num % LONGCARD(base))];
         num = num / LONGCARD(base);
      }
      while (num != 0);

     /* copy conversion result from str[i..HIGH(str)] to str[0..] */
      j = 0;
      while (i <= HIGH(str)) {
         str[j] = str[i];
         INC(i);
         INC(j);
      }
      if (j <= HIGH(str)) {  /* make ASCIIZ */
         str[j] = '\0';
      }
   } // WholeToStr

   void Bool(_ARRAYOF<CHAR>& s, BOOLEAN b) {
      /* by Zoran Budimac, February, 1995 */
      if (b && (HIGH(s)>=3))
         String::Copy(s, "TRUE");
      else if (!b && (HIGH(s)>=4))
         String::Copy(s, "FALSE");
      else
         SetError(noRoomError, "Bool");
   } // Bool

   void Int(_ARRAYOF<CHAR>& s, INTEGER n) {
      IntB(s, n, 10);
   } // Int

   void LongInt(_ARRAYOF<CHAR>& s, LONGINT n) {
      LongIntB(s, n, 10);
   } // LongInt

   void Card(_ARRAYOF<CHAR>& s, CARDINAL n) {
      CardB(s, n, 10);
   } // Card

   void LongCard(_ARRAYOF<CHAR>& s, LONGCARD n) {
      LongCardB(s, n, 10);
   } // LongCard

   void Real(_ARRAYOF<CHAR>& s, REAL x) {
      ARRAY<_SubrangeType<CARDINAL, 0, 200>, CHAR> str;

      sprintf((char*)_DATAADR(str), "%E", x);
      if (String::Length(str) > HIGH(s)+1)
         SetError(noRoomError, "Real");
      else
         String::Copy(s, str);
   } // Real

   void LongReal(_ARRAYOF<CHAR>& s, LONGREAL x) {
      ARRAY<_SubrangeType<CARDINAL, 0, 200>, CHAR> str;

      sprintf((char*)_DATAADR(str), "%E", x);
      if (String::Length(str) > HIGH(s)+1)
         SetError(noRoomError, "LongReal");
      else
         String::Copy(s, str);
   } // LongReal

   void IntB(_ARRAYOF<CHAR>& s, INTEGER n, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      BOOLEAN neg, ok;
      if ((base >= minBase) && (base <= maxBase)) {
         neg = n < 0;
         WholeToStr(LONGCARD(ABS(n)), base, s, ok);
         if (!ok) SetError(noRoomError, "IntB");
         if (neg) String::Insert(s, "-", 0);
      }
      else
         SetError(wrongBaseError, "IntB");
   } // IntB

   void LongIntB(_ARRAYOF<CHAR>& s, LONGINT n, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      BOOLEAN neg, ok;
      if ((base >= minBase) && (base <= maxBase)) {
         neg = n < 0;
         WholeToStr(LONGCARD(ABS(n)), base, s, ok);
         if (!ok) SetError(noRoomError, "LongIntB");
         if (neg) String::Insert(s, "-", 0);
      }
      else
         SetError(wrongBaseError, "LongIntB");
   } // LongIntB

   void CardB(_ARRAYOF<CHAR>& s, CARDINAL n, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      BOOLEAN ok;
      if ((base >= minBase) && (base <= maxBase)) {
         WholeToStr(LONGCARD(n), base, s, ok);
         if (!ok) SetError(noRoomError, "CardB");
      }
      else
         SetError(wrongBaseError, "CardB");
   } // CardB

   void LongCardB(_ARRAYOF<CHAR>& s, LONGCARD n, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      BOOLEAN ok;
      if ((base >= minBase) && (base <= maxBase)) {
         WholeToStr(n, base, s, ok);
         if (!ok) SetError(noRoomError, "LongCardB");
      }
      else
         SetError(wrongBaseError, "LongCardB");
   } // LongCardB

   void Empty(_ARRAYOF<CHAR>& s, CARDINAL n, CHAR ch) {
      /* 's' is a string consisting 'n' characters 'ch' */
      /* by Zoran Budimac, February, 1995.
         Modified by Tamas Cservenak, May, 1996 */
      ARRAY<_SubrangeType<CARDINAL, 0, 0>, CHAR> str; /* String to hold character 'ch' */

      str[0] = ch;
      String::Copy(s, "");
      for (CARDINAL i = 1; i <= n; INC(i)) String::Append(s, str);
   } // Empty

   void PadL(_ARRAYOF<CHAR>& s1, _ARRAYOF<CHAR> s,
             CARDINAL n, CHAR ch) {
      /* by Zoran Budimac, February, 1995 */
      CARDINAL l;
      ARRAY<_SubrangeType<CARDINAL, 0, 255>, CHAR> t;

      l = String::Length(s);
      if ((l > n) || (l > HIGH(s1)+1) || (n > HIGH(s1)+1))
         SetError(noRoomError, "PadL");
      else {
         Empty(t, n-l, ch);
         String::Concat(s1, s, t);
      }
   } // PadL

   void PadR(_ARRAYOF<CHAR>& s1, _ARRAYOF<CHAR> s,
             CARDINAL n, CHAR ch) {
      /* by Zoran Budimac, February, 1995 */
      CARDINAL l;
      ARRAY<_SubrangeType<CARDINAL, 0, 255>, CHAR> t;

      l = String::Length(s);
      if ((l > n) || (l > HIGH(s1)+1) || (n > HIGH(s1)+1))
         SetError(noRoomError, "PadR");
      else {
         Empty(t, n-l, ch);
         String::Concat(s1, t, s);
      }
   } // PadR

   void PadC(_ARRAYOF<CHAR>& s1, _ARRAYOF<CHAR> s,
             CARDINAL n, CHAR ch) {
      /* by Zoran Budimac, February, 1995 */
      CARDINAL l;
      ARRAY<_SubrangeType<CARDINAL, 0, 255>, CHAR> t;

      l = String::Length(s);
      if ((l > n) || (l > HIGH(s1)+1) || (n > HIGH(s1)+1))
         SetError(noRoomError, "PadC");
      else {
         Empty(t, (n-l) / 2, ch);
         String::Concat(s1, t, s);
         Empty(t, n - l - (n-l) / 2, ch);
         String::Concat(s1, s1, t);
      }
   } // PadC

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // Fmt

#endif
