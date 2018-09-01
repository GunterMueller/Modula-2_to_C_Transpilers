/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       String (1.1)                                *
 * Version:                 1.2.0                                       *
 * Date:                    February 19, 2000                           *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module is portable.                    *
 *                                                                      *
 * Author:                  Tamas Cservenak & Jasmina Vesic             *
 *                              University of Novi Sad, Yugoslavia      *
 * Modifications:           Zoran Budimac                               *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_String_cpp_
#define _M2_String_cpp_

#include <_m2sys.h>

#include <String.m2h>

#include <Wr.m2h>
#include <Wr.cpp>

namespace String {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** String::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case noRoomError:
            Wr::Str("No room in destination string");
            break;
         case wrongPositionError:
            Wr::Str("Invalid position in a string was requested");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error


   INTEGER Comp(_ARRAYOF<CHAR> s, _ARRAYOF<CHAR> p) {
      /* by Tamas Cservenak, May, 1996 */
      CARDINAL i = 0;

      while ((i <= HIGH(s)) && (i <= HIGH(p))) {
         if (s[i] > p[i]) {
            return 1;
         }
         else if (s[i] < p[i]) {
            return -1;
         }
         else if (s[i] == '\0') { /* implicitly s[i] == p[i] == '\0'! */
            return 0;
         }
         INC(i);
      }
      if (i <= HIGH(s)) {
         if (s[i] == '\0') {
            return 0;
         }
         else {
            return 1;
         }
      }
      else if (i <= HIGH(p)) {
         if (p[i] == '\0') {
            return 0;
         }
         else {
            return -1;
         }
      }
      else {
         return 0;
      }
   } // Comp

   CARDINAL Length(_ARRAYOF<CHAR> s) {
      return LENGTH(s);
   } // Length

   BOOLEAN SubString(_ARRAYOF<CHAR> s, _ARRAYOF<CHAR> p, CARDINAL& pos) {
      /* by Jasmina Vesic, November, 1997 */
      CARDINAL i, j, k, plen, slen;

      plen = Length(p);
      slen = Length(s);
      i = 0;
      for (;;) {
         if (i == slen) return FALSE;
         j = 0;
         k = i;
         for (;;) {
            if (j == plen) {
               pos = i;
               return TRUE;
            }
            if (k == slen) return FALSE;
            if (s[k] != p[j]) break;
            INC(j);
            INC(k);
         }
      INC(i);
      }
   } // SubString

   void Caps(_ARRAYOF<CHAR>& s) {
      /* by Jasmina Vesic, November 1997 */
      /* modified by Milos Radovanovic, February 2000 */
      CARDINAL i = 0;
      while ((s[i] != '\0') && (i <= HIGH(s))) {
         s[i] = CAP(s[i]);
         INC(i);
      }
   } // Caps

   void Append(_ARRAYOF<CHAR>& s, _ARRAYOF<CHAR> p) {
      /* by Tamas Cservenak, May, 1996.
      Modified by Zoran Budimac, July, 1996 */
      /* modified by Milos Radovanovic, February 2000 */
      CARDINAL slen, plen;

      slen = Length(s);
      plen = Length(p);
      if (slen + plen > HIGH(s)+1) {
         SetError(noRoomError, "Append");
         return;
      }
      for (CARDINAL i = 0; i <= plen - 1; INC(i)) {
         s[slen+i] = p[i];
      }
      if (slen+plen <= HIGH(s)) s[slen+plen] = '\0';
   } // Append

   void Concat(_ARRAYOF<CHAR>& s, _ARRAYOF<CHAR> s1, _ARRAYOF<CHAR> s2) {
      /* by Jasmina Vesic, Novemeber, 1997 */
      /* modified by Milos Radovanovic, February 2000 */
      CARDINAL i, j, s1len, s2len;

      s1len = Length(s1);
      s2len = Length(s2);
      if (s1len + s2len > HIGH(s)+1) {
         SetError(noRoomError, "Concat");
         return;
      }
      i = 0;
      while (i < s1len) {
         if (i > HIGH(s)) {
            s[i] = '\0';
            return;
         }
         s[i] = s1[i];
         INC(i);
      }
      j = 0;
      while (j < s2len) {
         if (i > HIGH(s)) {
            s[i] = '\0';
            return;
         }
         s[i] = s2[j];
         INC(i);
         INC(j);
      }
      if (i <= HIGH(s)) s[i] = '\0';
   } // Concat

   void Copy(_ARRAYOF<CHAR>& s, _ARRAYOF<CHAR> p) {
      /* by Jasmina Vesic, November 1997 */
      /* modified by Milos Radovanovic, February 2000 */
      CARDINAL plen = Length(p);

      if (plen > HIGH(s)+1) {
         SetError(noRoomError, "Copy");
         return;
      }
      for (CARDINAL i = 0; i < plen; INC(i)) {
         s[i] = p[i];
      }
      if (plen <= HIGH(s)) s[plen] = '\0';
   } // Copy

   BOOLEAN Equal(_ARRAYOF<CHAR> s, _ARRAYOF<CHAR> p) {
      return ((Comp(s, p) == 0) ? TRUE : FALSE);
   } // Equal

   BOOLEAN LEq(_ARRAYOF<CHAR> s, _ARRAYOF<CHAR> p) {
      return ((Comp(s, p) <= 0) ? TRUE : FALSE);
   } // LEq

   BOOLEAN GEq(_ARRAYOF<CHAR> s, _ARRAYOF<CHAR> p) {
      return ((Comp(s, p) >= 0) ? TRUE : FALSE);
   } // Geq

   void Insert(_ARRAYOF<CHAR>& s, _ARRAYOF<CHAR> p, CARDINAL pos) {
      /* by Zoran Budimac, July, 1996 */
      CARDINAL i, j, c, l;

      l = Length(s);
      i = Length(p);

      if (pos > l) {
         SetError(wrongPositionError, "Insert");
         return;
      }
      if (i + l > HIGH(s)+1) {
         SetError(noRoomError, "Insert");
         return;
      }
      c = l;
      if (c < pos) pos = c;
      DEC(c, pos);
      for (LONGINT ji = c; ji >= 0; DEC(ji)) {
         if (ji + pos + i <= HIGH(s)) s[ji+pos+i] = s[ji+pos];
      }
      j = 0;
      while ((j < i) && (pos + j <= HIGH(s))) {
         s[pos+j] = p[j];
         INC(j);
      }
   } // Insert

   void Delete(_ARRAYOF<CHAR>& s, CARDINAL from, CARDINAL to) {
      /* by Tamas Cservenak, May, 1996 */
      /* modified by Milos Radovanovic, February 2000 */
      CARDINAL len, shft;

      len = Length(s);
      if ((len == 0) || (len < from) || (len < to) || (from > to)) {
         SetError(wrongPositionError, "Delete");
         return;
      }
      shft = to - from + 1;
      for (CARDINAL i = from; i <= len - 1; INC(i)) { /* INCLUSIVE */
         if (i + shft <= len - 1) {
            s[i] = s[i+shft];
         }
         else {
            s[i] = '\0';
            //break; // proveri!
         }
      }
   } // Delete

   void Slice(_ARRAYOF<CHAR>& s, _ARRAYOF<CHAR> p,
              CARDINAL from, CARDINAL to) {
      /* by Tamas Cservenak, July, 1996. Debugged by Jasmina Vesic. */
      /* modified by Milos Radovanovic, February 2000 */
      CARDINAL len;

      len = Length(p);
      if ((len == 0) || (len < from) || (len < to) || (from > to)) {
         SetError(wrongPositionError, "Slice");
         return;
      }
      if (to - from > HIGH(s)) {
         SetError(noRoomError, "Slice");
         return;
      }
      for (CARDINAL i = from; i <= to; INC(i)) {
         if (i - from > HIGH(s)) return;
         s[i-from] = p[i];
      }
      if (to - from + 1 <= HIGH(s)) s[to-from+1] = '\0';
   } // Slice

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // String

#endif
