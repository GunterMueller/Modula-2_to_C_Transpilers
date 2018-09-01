/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Time (1.1)                                  *
 * Version:                 1.0                                         *
 * Date:                    June 8, 2000                                *
 * Conformance Test Passed: -                                           *
 * Platform:                DOS compatible                              *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             Non-portable operations are contained in    *
 *                          separate module (DOSTimes). To port this    *
 *                          module, provide XXXTimes module for XXX     *
 *                          platform.                                   *
 *                                                                      *
 * Author:                  Zoran Budimac                               *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Time_cpp_
#define _M2_Time_cpp_

#include <_m2sys.h>

#include <Time.m2h>

#include <Fmt.m2h>
#include <Scan.m2h>
#include <String.m2h>
#include <Wr.m2h>
#include <DOSTimes.m2h>
#include <Fmt.cpp>
#include <Scan.cpp>
#include <String.cpp>
#include <Wr.cpp>
#include <DOSTimes.cpp>

namespace Time {

   using DOSTimes::SetST;
   using DOSTimes::GetST;

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** Time::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case noRoomError:
            Wr::Str("No room in destination string");
            break;
         case conversionError:
            Wr::Str("Conversion error");
            break;
         case wrongTimeError:
            Wr::Str("Wrong time");
            break;
         case priviledgeError:
            Wr::Str("Not enough privilegdes to get/set time");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error

   BOOLEAN OK(T t) {
      /* Returns TRUE if time 't' is a valid time */
      /* by Zoran Budimac, February, 1995 */
     return (Hour(t) >= 0) && (Hour(t) <= 23) &&
            (Min(t) >= 0) && (Min(t) <= 59) &&
            (Sec(t) >= 0) && (Sec(t) <= 59) &&
            (Hund(t) >= 0) && (Hund(t) <= 99);
   } // OK

   CARDINAL Hund(T t) {
      /* by Zoran Budimac, February, 1995 */
      return t.hun;
   } // Hund

   CARDINAL Sec(T t) {
      /* by Zoran Budimac, February, 1995 */
      return t.sec;
   } // Sec

   CARDINAL Min(T t) {
      /* by Zoran Budimac, February, 1995 */
      return t.min;
   } // Min

   CARDINAL Hour(T t) {
      /* by Zoran Budimac, February, 1995 */
      return t.hour;
   } // Hour

   void SetHund(T& t, CARDINAL n) {
      /* by Zoran Budimac, February, 1995 */
      if (n > 99) SetError(wrongTimeError, "SetHund");
      t.hun = n; // ToDo: razmisli o else
   } // SetHund

   void SetSec(T& t, CARDINAL n) {
      /* by Zoran Budimac, February, 1995 */
      if (n > 59) SetError(wrongTimeError, "SetSec");
      t.sec = n;
   } // SetSec

   void SetMin(T& t, CARDINAL n) {
      /* by Zoran Budimac, February, 1995 */
      if (n > 59) SetError(wrongTimeError, "SetMin");
      t.min = n;
   } // SetMin

   void SetHour(T& t, CARDINAL n) {
      /* by Zoran Budimac, February, 1995 */
      if (n > 23) SetError(wrongTimeError, "SetHour");
      t.hour = n;
   } // SetHour

   void Init(T& t, CARDINAL hh, CARDINAL mm, CARDINAL ss, CARDINAL hd) {
      /* by Zoran Budimac, February, 1995 */
      t.hun  = hh;
      t.sec  = mm;
      t.min  = ss;
      t.hour = hd;
      if (!OK(t)) SetError(wrongTimeError, "Init");
   } // Init

   void Copy(T& t, T t1) {
      /* by Zoran Budimac, July, 1996 */
      t = t1;
   } // Copy
      /* Copies the time 't1' into the time 't' */

   void SetSystemTime(T t) {
      BOOLEAN ok;
      SetST(t, ok);
   } // SetSystemTime

   void GetSystemTime(T& t) {
      BOOLEAN ok;
      GetST(t, ok);
   } // GetSystemTime

   void ToString(_ARRAYOF<CHAR>& str, T t) {
      /* by Zoran Budimac, February, 1995 */
      ARRAY <_SubrangeType<CARDINAL, 0, 20>, CHAR> tmp;

      /* Errors. The order is important */
      if (10 > HIGH(str)) SetError(noRoomError, "ToString");
      Fmt::Card(tmp, t.hour);
      Fmt::PadR(tmp, tmp, 2, '0');
      String::Concat(str, tmp, ":");
      Fmt::Card(tmp, t.min);
      Fmt::PadR(tmp, tmp, 2, '0');
      String::Append(str, tmp);
      String::Append(str, ":");
      Fmt::Card(tmp, t.sec);
      Fmt::PadR(tmp, tmp, 2, '0');
      String::Append(str, tmp);
      String::Append(str, '.');
      Fmt::Card(tmp, t.hun);
      Fmt::PadR(tmp, tmp, 2, '0');
      String::Append(str, tmp);
   } // ToString

   void FromString(T& t, _ARRAYOF<CHAR> str) {
      /* by Zoran Budimac, February, 1995 */
      ARRAY <_SubrangeType<CARDINAL, 0, 20>, CHAR> tmp;
      CARDINAL pos, n;

      Init(t, 0, 0, 0, 0);

      /* Hours */
      if (!String::SubString(str, ":", pos))
         SetError(conversionError, "FromString");

      String::Slice(tmp, str, 0, pos);
      n = Scan::Card(tmp);
      SetHour(t, n);
      String::Delete(str, 0, pos+1);

      /* Minutes */
      if (!String::SubString(str, ":", pos))
         /* String was in the form 'hh:mm' */
         return;

      String::Slice(tmp, str, 0, pos);
      n = Scan::Card(tmp);
      SetMin(t, n);
      String::Delete(str, 0, pos+1);

      /* Seconds */
      if (!String::SubString(str, ".", pos))
         /* String was in the form 'hh:mm' */
         return;

      String::Slice(tmp, str, 0, pos);
      n = Scan::Card(tmp);
      SetSec(t, n);
      String::Delete(str, 0, pos+1);

      /* Hundreds */
      n = Scan::Card(str);
      SetHund(t, n);
      if (!OK(t)) SetError(wrongTimeError, "FromString");
   } // FromString

   LONGCARD Hundts(T t) {
      /* by Zoran Budimac, February, 1995 */
      return 360000*LONGCARD(t.hour) + 6000*LONGCARD(t.min) +
             100*LONGCARD(t.sec) + LONGCARD(t.hun);
   } // Hundts

   void FromHundts(T& t, LONGCARD hundts) {
      /* by Zoran Budimac, February, 1995 */
      t.hun  = CARDINAL(hundts % 100);
      hundts = hundts / 100;
      t.sec  = CARDINAL(hundts % 60);
      hundts = hundts / 60;
      t.min  = CARDINAL(hundts % 60);
      hundts = hundts / 60;
      t.hour = CARDINAL(hundts % 60);
      if(!OK(t)) SetError(wrongTimeError, "FromHundts");
   } // FromHundts

   void Inc(T& t, LONGCARD hun) {
      /* by Zoran Budimac, February, 1995 */
      FromHundts(t, Hundts(t) + hun);
   } // Inc

   void Dec(T& t, LONGCARD hun) {
      /* by Zoran Budimac, February, 1995 */
      FromHundts(t, Hundts(t) - hun);
   } // Dec

   void Add(T& t, T t1, T t2) {
      /* by Zoran Budimac, February, 1995 */
      FromHundts(t, Hundts(t1) + Hundts(t2));
   } // Add

   void Sub(T& t, T t1, T t2) {
      /* by Zoran Budimac, February, 1995 */
      FromHundts(t, Hundts(t1) - Hundts(t2));
   } // Sub

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // Time

#endif
