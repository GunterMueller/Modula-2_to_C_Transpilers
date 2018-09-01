/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Date (1.1)                                  *
 * Version:                 1.0                                         *
 * Date:                    June 8, 2000                                *
 * Conformance Test Passed: -                                           *
 * Platform:                DOS compatible                              *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             Non-portable operations are contained in    *
 *                          separate module (DOSDates). To port this    *
 *                          module, provide XXXDates module for XXX     *
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

#ifndef _M2_Date_cpp_
#define _M2_Date_cpp_

#include <_m2sys.h>

#include <Date.m2h>

#include <Fmt.m2h>
#include <Scan.m2h>
#include <String.m2h>
#include <Wr.m2h>
#include <DOSDates.m2h>
#include <Fmt.cpp>
#include <Scan.cpp>
#include <String.cpp>
#include <Wr.cpp>
#include <DOSDates.cpp>

namespace Date {

   using DOSDates::SetSD;
   using DOSDates::GetSD;

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** Date::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case noRoomError:
            Wr::Str("No room in destination string");
            break;
         case conversionError:
            Wr::Str("Conversion error");
            break;
         case wrongDateError:
            Wr::Str("Wrong date");
            break;
         case priviledgeError:
            Wr::Str("Not enough privilegdes to get/set date");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error

   BOOLEAN IsLeapYear(CARDINAL year) {
      /* by Zoran Budimac, February, 1995 */
      return ((year % 100 != 0) && (year % 4 == 0)) || (year % 400 == 0);
   } // IsLeapYear

   CARDINAL DaysInMonth(CARDINAL m, CARDINAL y) {
      /* Returns the number of days in a month 'm' (of a year 'y') */
      /* by Zoran Budimac, February, 1995 */
      switch (m) {
      case 1: case 3: case 5: case 7: case 8: case 10: case 12:
         return 31;
      case 4: case 6: case 9: case 11:
         return 30;
      case 2:
         return 28 + ORD(IsLeapYear(y));  /* 0 if FALSE, 1 if TRUE */
      default:
         SetError(wrongDateError, "DaysInMonth");
         return 0;
      }
   } // DaysInMonth

   CARDINAL DaysInYear(CARDINAL y) {
      /* Returns the number of days in a year 'y' */
      /* by Zoran Budimac, February 1995 */
      return 365 + ORD(IsLeapYear(y));    /* 0 if FALSE, 1 if TRUE */
   } // DaysInYear

   CARDINAL DayNoOfYear(T d) {
      /* Returns the ordinal number of a day of date 'd' in a year */
      /* by Zoran Budimac, February 1995 */
      CARDINAL i, s;

      s = Day(d);
      for (i = 1; i <= Month(d)-1; INC(i)) INC(s, DaysInMonth(i, Year(d)));
      return s;
   } // DayNoOfYear

   BOOLEAN OK(T d) {
      /* Returns TRUE if date 'd' is OK */
      /* by Zoran Budimac, February 1995 */
      return (Day(d) >=1)    && (Day(d) <= DaysInMonth(Month(d), Year(d))) &&
             (Month(d) >= 1) && (Month(d) <= 12);
   } // OK

   CARDINAL Day(T d) {
      return d.day;
   } // Day

   CARDINAL Month(T d) {
      return d.month;
   } // Month

   CARDINAL Year(T d) {
      return d.year;
   } // Year

   void SetDay(T& d, CARDINAL n) {
      if ((n > 31) || (n < 1)) SetError(wrongDateError, "SetDay");
      d.day = n;
   } // SetDay

   void SetMonth(T& d, CARDINAL n) {
      if ((n > 12) || (n < 1)) SetError(wrongDateError, "SetMonth");
      d.month = n;
   } // SetMonth

   void SetYear(T& d, CARDINAL n) {
      d.year = n;
   } // SetYear

   void Init(T& d, CARDINAL dd, CARDINAL mm, CARDINAL yy) {
      d.day   = dd;
      d.month = mm;
      d.year  = yy;
      if (!OK(d)) SetError(wrongDateError, "Init");
   } // Init

   void Copy(T& d, T d1) {
      d = d1;
   } // Copy

   void SetSystemDate(T d) {
      BOOLEAN ok;
      SetSD(d, ok);
   } // SetSystemDate

   void GetSystemDate(T& d) {
      BOOLEAN ok;
      GetSD(d, ok);
   } // GetSystemDate

   WeekDay DayOfWeek(T date) {
      /* by Zoran Budimac, February, 1996 */
      CARDINAL d, m, y, xx, yy, zz;

      d = Day(date); m = Month(date); y = Year(date);
      if ((m == 1) || (m == 2)) {
         INC(m, 10);
         DEC(y);
      }
      else
         DEC(m, 2);

      yy = y % 100;
      zz = y / 100;
      xx = TRUNC(2.6*FLOAT(m)-0.2) + d + yy + (y / 4) + (zz / 4) - 2*zz;
      xx = xx % 7;
      return xx;
   } // DayOfWeek

   void ToString(_ARRAYOF<CHAR>& str, T d) {
      ARRAY <_SubrangeType<CARDINAL, 0, 20>, CHAR> tmp;

      if (10 > HIGH(str)) SetError(noRoomError, "ToString");
      String::Copy(str, "");
      Fmt::Card(tmp, d.year);
      Fmt::PadR(tmp, tmp, 4, '0');
      String::Append(str, tmp);
      String::Append(str, '/');
      Fmt::Card(tmp, d.month);
      Fmt::PadR(tmp, tmp, 2, '0');
      String::Append(str, tmp);
      String::Append(str, '/');
      Fmt::Card(tmp, d.day);
      Fmt::PadR(tmp, tmp, 2, '0');
      String::Append(str, tmp);
   } // ToString

   void FromString(T& d, _ARRAYOF<CHAR> str) {
      /* by Zoran Budimac, February, 1995 */
      ARRAY <_SubrangeType<CARDINAL, 0, 10>, CHAR> tmp;
      CARDINAL pos, n;

      Init(d, 0, 0, 0);

      /* Year */
      if (!String::SubString(str, '/', pos))
         SetError(conversionError, "FromString");

      String::Slice(tmp, str, 0, pos);
      n = Scan::Card(tmp);
      SetYear(d, n);
      String::Delete(str, 0, pos+1);

      /* Month */
      if (!String::SubString(str, '/', pos))
         SetError(conversionError, "FromString");

      String::Slice(tmp, str, 0, pos);
      n = Scan::Card(tmp);
      SetMonth(d, n);
      String::Delete(str, 0, pos+1);

      /* Days */
      n = Scan::Card(str);
      SetDay(d, n);
      if (!OK(d)) SetError(wrongDateError, "FromString");
   } // FromString

   LONGCARD Days(T d) {
      /* by Zoran Budimac, February, 1996 */
      CARDINAL i;
      LONGCARD s;

      s = 0;
      for (i = 1582; i <= Year(d)-1; INC(i))
         INC(s, LONGCARD(DaysInYear(i)));

      INC(s, LONGCARD(DayNoOfYear(d)));
      return s;
   } // Days

   void FromDays(T& d, LONGCARD days) {
      /* Naive version, by Zoran Budimac, February, 1996 */
      CARDINAL y, m;

      y = 1581; m = 0;
      while (days > LONGCARD(DaysInYear(y+1))) {
         INC(y);
         DEC(days, LONGCARD(DaysInYear(y)));
      }
      INC(y);
      while (days > LONGCARD(DaysInMonth(m+1, y))) {
         INC(m);
         DEC(days, LONGCARD(DaysInMonth(m, y)));
      }
      INC(m);
      Init(d, CARDINAL(days), m, y);
   } // FromDays

   void Inc(T& d1, LONGCARD days) {
      FromDays(d1, Days(d1) + days);
   } // Inc

   void Dec(T& d1, LONGCARD days) {
      FromDays(d1, Days(d1) - days);
   } // Dec

   void Add(T& d, T d1, T d2) {
      FromDays(d, Days(d1) + Days(d2));
   } // Add

   void Sub(T& d, T d1, T d2) {
      FromDays(d, Days(d1) - Days(d2));
   } // Sub

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // Date

#endif
