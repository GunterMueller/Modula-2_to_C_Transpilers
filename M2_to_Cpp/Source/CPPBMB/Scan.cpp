/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Scan (1.1)                                  *
 * Version:                 1.0                                         *
 * Date:                    June 6, 2000                                *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 *                                                                      *
 * Authors:                 Tamas Cservenak & Zoran Budimac             *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: {icervenak,zjb}@unsim.ns.ac.yu  *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Scan_cpp_
#define _M2_Scan_cpp_

#include <stdlib.h>
#include <errno.h>

#include <_m2sys.h>

#include <Scan.m2h>

#include <String.m2h>
#include <Wr.m2h>
#include <String.cpp>
#include <Wr.cpp>

namespace Scan {

   const CARDINAL minBase = 2;
   const CARDINAL maxBase = 16;

   CARDINAL errorCode = noError;

   ARRAY <_SubrangeType<CARDINAL, 0, 15>, CHAR> digitChars = "0123456789ABCDEF";

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** Scan::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case conversionError:
            Wr::Str("Conversion error");
            break;
         case overflowError:
            Wr::Str("Overflow error");
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

   void StrToWhole(LONGCARD& num, Base base, _ARRAYOF<CHAR> str,
                        BOOLEAN& done, BOOLEAN& overflow) {
      /* Converts 'str' to 'num' using 'base'. 'done' is TRUE if
         everything went OK. 'overflow' is TRUE if overflow occured */
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */

      CARDINAL i;
      CARDINAL digit;
      CHAR ch;

      overflow = FALSE;
      if (String::Length(str) == 0) {
         done = FALSE;
         return;
      }
      i   = 0;
      num = 0;
      for (;;) {
         if (i <= HIGH(str))
            ch = CAP(str[i]);
         else
            ch = '\0';

         if (('0' <= ch) && (ch <= '9'))
            digit = ORD(ch) - ORD('0');
         else if (('A' <= ch) && (ch <= 'F'))
            digit = ORD(ch) - ORD('A') + 10;
         else
            break;

         INC(i);

         if ((digit < base) && (num <= (MAX(LONGCARD) - LONGCARD(digit)) / LONGCARD(base)))
            num = num * LONGCARD(base) + LONGCARD(digit);
         else if (digit >= base) /* bad digit! */
            break;
         else {                  /* overflow! */
            overflow = TRUE;
            break;
         }
      }
      done = ch == '\0';
   } // StrToWhole

   BOOLEAN Bool(_ARRAYOF<CHAR> s) {
      /* by Zoran Budimac, July, 1995 */
      CARDINAL pos;

      if (String::SubString(s, "TRUE", pos) && (pos == 0))
         return TRUE;
      else if (String::SubString(s, "FALSE", pos) && (pos == 0))
         return FALSE;
      else {
         SetError(conversionError, "Bool");
         return FALSE;
      }
   } // Bool

   INTEGER Int(_ARRAYOF<CHAR> s) {
      return IntB(s, 10);
   } // Int

   LONGINT LongInt(_ARRAYOF<CHAR> s) {
      return LongIntB(s, 10);
   } // LongInt

   CARDINAL Card(_ARRAYOF<CHAR> s) {
      return CardB(s, 10);
   } // Card

   LONGCARD LongCard(_ARRAYOF<CHAR> s) {
      return LongCardB(s, 10);
   } // LongCard

   REAL Real(_ARRAYOF<CHAR> s) {
      CHAR *ptr = _CSTR(s);
      CHAR *endptr = ptr;
      REAL n;

      errno = 0;
      n = strtod(ptr, &endptr);

      if (errno != 0)
         SetError(overflowError, "Real");
      else if ((n == 0.0) && (endptr == ptr))
         SetError(conversionError, "Real");

      return n;
   } // Real

   LONGREAL LongReal(_ARRAYOF<CHAR> s) {
      CHAR *ptr = _CSTR(s);
      CHAR *endptr = ptr;
      LONGREAL n;

      errno = 0;
      n = strtod(ptr, &endptr);

      if (errno != 0)
         SetError(overflowError, "LongReal");
      else if ((n == 0.0) && (endptr == ptr))
         SetError(conversionError, "LongReal");

      return n;
   } // LongReal

   INTEGER IntB(_ARRAYOF<CHAR> s, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      LONGCARD n;
      BOOLEAN ok, over, neg;
      CARDINAL p;

      neg = String::SubString(s, "-", p);
      if (neg) String::Delete(s, p, p);
      StrToWhole(n, base, s, ok, over);

      /* Errors. The order is important! */
      if ((over) ||
          neg  && (n > LONGCARD(MAX(INTEGER))+1) ||
          !neg && (n > LONGCARD(MAX(INTEGER)))
         )
         SetError(overflowError, "IntB");

      if ((!ok) && (!over))
         SetError(conversionError, "IntB");

      if ((base < minBase) || (base > maxBase))
         SetError(wrongBaseError, "IntB");

      if (neg)
         return -1 * INTEGER(n);
      else
         return INTEGER(n);
   } // IntB

   LONGINT LongIntB(_ARRAYOF<CHAR> s, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      LONGCARD n;
      BOOLEAN ok, over, neg;
      CARDINAL p;

      neg = String::SubString(s, "-", p);
      if (neg) String::Delete(s, p, p);
      StrToWhole(n, base, s, ok, over);

      /* Errors. The order is important! */
      if ((over) ||
          neg  && (n > LONGCARD(MAX(LONGINT))+1) ||
          !neg && (n > LONGCARD(MAX(LONGINT)))
         )
         SetError(overflowError, "LongIntB");

      if ((!ok) && (!over))
         SetError(conversionError, "LongIntB");

      if ((base < minBase) || (base > maxBase))
         SetError(wrongBaseError, "LongIntB");

      if (neg)
         return -1 * LONGINT(n);
      else
         return LONGINT(n);
   } // LongIntB

   CARDINAL CardB(_ARRAYOF<CHAR> s, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      LONGCARD n;
      BOOLEAN ok, over;

      StrToWhole(n, base, s, ok, over);

      /* Errors. The order is important! */
      if (over || (n > MAX(CARDINAL)))
         SetError(overflowError, "CardB");

      if ((!ok) && (!over))
         SetError(conversionError, "CardB");

      if ((base < minBase) || (base > maxBase))
         SetError(wrongBaseError, "CardB");

      return CARDINAL(n);
   } // CardB

   LONGCARD LongCardB(_ARRAYOF<CHAR> s, Base base) {
      /* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 */
      LONGCARD n;
      BOOLEAN ok, over;

      StrToWhole(n, base, s, ok, over);

      /* Errors. The order is important! */
      if (over)
         SetError(overflowError, "LongCardB");

      if ((!ok) && (!over))
         SetError(conversionError, "LongCardB");

      if ((base < minBase) || (base > maxBase))
         SetError(wrongBaseError, "LongCardB");

      return n;
   } // LongCardB

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // Scan

#endif
