(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Date (1.1)                                *
 * Version:                 1.0                                       *
 * Date:                    November 27, 1996                         *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * Portability:             Non-portable operations are contained in  *
 *                          separate module (TS1Dates). To port this  *
 *                          module, provide XXXDates module for XXX   *
 *                          platform.
 *                                                                    *
 * Author:                      Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Date;
IMPORT Fmt, Scan, String, Wr;
FROM TS1Dates IMPORT SetSD, GetSD;
              (* Operations characteristic for Top Speed M2/80x86/MS-DOS *)

VAR
   errorCode : CARDINAL;

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg);
END SetError;

PROCEDURE IsLeapYear(year: CARDINAL): BOOLEAN;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN ((year MOD 100 # 0) AND (year MOD 4 = 0)) OR (year MOD 400 = 0)
END IsLeapYear;

PROCEDURE DaysInMonth(m, y: CARDINAL): CARDINAL;
   (* Returns the number of days in a month 'm' (of a year 'y') *)
   (* by Zoran Budimac, February, 1995 *)
BEGIN
   CASE m OF
     1, 3, 5, 7, 8, 10, 12: RETURN 31 |
     4, 6, 9, 11: RETURN 30 |
     2: RETURN 28 + ORD(IsLeapYear(y))   (* 0 if FALSE, 1 if TRUE *)
   END;
END DaysInMonth;

PROCEDURE DaysInYear(y: CARDINAL): CARDINAL;
   (* Returns the number of days in a year 'y' *)
   (* by Zoran Budimac, February 1995 *)
BEGIN
   RETURN 365 + ORD(IsLeapYear(y))   (* 0 if FALSE, 1 if TRUE *)
END DaysInYear;

PROCEDURE DayNoOfYear(d: T): CARDINAL;
   (* Returns the ordinal number of a day of date 'd' in a year *)
   (* by Zoran Budimac, February 1995 *)
VAR i, s: CARDINAL;
BEGIN
   s := Day(d);
   FOR i := 1 TO Month(d)-1 DO
     INC(s, DaysInMonth(i, Year(d)));
   END;
   RETURN s
END DayNoOfYear;

PROCEDURE OK(d: T): BOOLEAN;
   (* Returns TRUE if date 'd' is OK *)
   (* by Zoran Budimac, February 1995 *)
VAR ok: BOOLEAN;
BEGIN
   RETURN (Day(d) >=1)    AND (Day(d) <= DaysInMonth(Month(d), Year(d))) AND
          (Month(d) >= 1) AND (Month(d) <= 12)
END OK;

PROCEDURE Day(d: T): CARDINAL;
BEGIN
  RETURN d.day
END Day;

PROCEDURE Month(d: T): CARDINAL;
BEGIN
  RETURN d.month
END Month;

PROCEDURE Year(d: T): CARDINAL;
BEGIN
  RETURN d.year
END Year;

PROCEDURE SetDay(VAR d: T; n: CARDINAL);
BEGIN
  IF (n > 31) OR (n < 1) THEN
     SetError(wrongDateError, "SetDay")
  END;
  d.day := n
END SetDay;

PROCEDURE SetMonth(VAR d: T; n: CARDINAL);
BEGIN
  IF (n > 12) OR (n < 1) THEN
     SetError(wrongDateError, "SetMonth")
  END;
  d.month := n
END SetMonth;

PROCEDURE SetYear(VAR d: T; n: CARDINAL);
BEGIN
  d.year := n
END SetYear;

PROCEDURE Init(VAR d: T; dd, mm, yy: CARDINAL);
BEGIN
  d.day   := dd;
  d.month := mm;
  d.year  := yy;
  IF NOT OK(d) THEN
     SetError(wrongDateError, "Init");
  END;
END Init;

PROCEDURE Copy(VAR t: T; t1: T);
BEGIN
  t := t1
END Copy;


PROCEDURE GetSystemDate(VAR d: T);
VAR ok: BOOLEAN;
BEGIN
   GetSD(d, ok)
END GetSystemDate;

PROCEDURE SetSystemDate(d: T);
VAR ok: BOOLEAN;
BEGIN
   SetSD(d, ok)
END SetSystemDate;

PROCEDURE DayOfWeek(date: T): WeekDay;
   (* by Zoran Budimac, February, 1996 *)
VAR d, m, y, xx, yy, zz : CARDINAL;
BEGIN
  d := Day(date); m := Month(date); y := Year(date);
  IF (m = 1) OR (m = 2) THEN
     INC(m, 10);
     DEC(y);
  ELSE
     DEC(m,2)
  END;
  yy := y MOD 100;
  zz := y DIV 100;
  xx := TRUNC(2.6*FLOAT(m)-0.2) + d + yy + (y DIV 4) + (zz DIV 4) - 2*zz;
  xx := xx MOD 7;
  RETURN xx
END DayOfWeek;

PROCEDURE ToString(VAR str: ARRAY OF CHAR; d: T);
VAR tmp: ARRAY [0..20] OF CHAR;
BEGIN
   IF 10 > HIGH(str) THEN SetError(noRoomError, "ToString") END;
   String.Copy(str, '');
   Fmt.Card(tmp, d.year);
   Fmt.PadR(tmp, tmp, 4, '0');
   String.Append(str, tmp);
   String.Append(str, '/');
   Fmt.Card(tmp, d.month);
   Fmt.PadR(tmp, tmp, 2, '0');
   String.Append(str, tmp);
   String.Append(str, '/');
   Fmt.Card(tmp, d.day);
   Fmt.PadR(tmp, tmp, 2, '0');
   String.Append(str, tmp);
END ToString;

PROCEDURE FromString(VAR d: T; str: ARRAY OF CHAR);
VAR tmp : ARRAY [0..10] OF CHAR;
    pos, n: CARDINAL;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  Init(d, 0, 0, 0);
  (* Year *)
  IF NOT String.SubString(str, '/', pos) THEN
     SetError(conversionError, "FromString")
  END;
  String.Slice(tmp, str, 0, pos);
  n := Scan.Card(tmp);
  SetYear(d, n);
  String.Delete(str, 0, pos+1);

  (* Month *)
  IF NOT String.SubString(str, '/', pos) THEN
     SetError(conversionError, "FromString")
  END;
  String.Slice(tmp, str, 0, pos);
  n := Scan.Card(tmp);
  SetMonth(d, n);
  String.Delete(str, 0, pos+1);

  (* Days *)
  n := Scan.Card(str);
  SetDay(d, n);
  IF NOT OK(d) THEN
     SetError(wrongDateError, "FromString");
  END;
END FromString;

PROCEDURE Days(d: T): LONGCARD;
VAR i: CARDINAL; s: LONGCARD;
   (* by Zoran Budimac, February, 1996 *)
BEGIN
  s := 0;
  FOR i := 1582 TO Year(d)-1 DO
    INC(s, LONGCARD(DaysInYear(i)))
  END;
  INC(s, LONGCARD(DayNoOfYear(d)));
  RETURN s
END Days;

PROCEDURE FromDays(VAR d: T; days: LONGCARD);
   (* Naive version, by Zoran Budimac, February, 1996 *)
VAR y, m: CARDINAL;
BEGIN
  y := 1581; m := 0;
  WHILE days > LONGCARD(DaysInYear(y+1)) DO
    INC(y);
    DEC(days, LONGCARD(DaysInYear(y)));
  END;

  INC(y);
  WHILE days > LONGCARD(DaysInMonth(m+1, y)) DO
    INC(m);
    DEC(days, LONGCARD(DaysInMonth(m, y)));
  END;
  INC(m);
  Init(d, CARDINAL(days), m, y);
END FromDays;

PROCEDURE Inc(VAR d1: T; days: LONGCARD);
BEGIN
  FromDays(d1, Days(d1) + days);
END Inc;

PROCEDURE Dec(VAR d1: T; days: LONGCARD);
BEGIN
  FromDays(d1, Days(d1) - days);
END Dec;

PROCEDURE Add(VAR d: T; d1, d2: T);
BEGIN
  FromDays(d, Days(d1) + Days(d2));
END Add;

PROCEDURE Sub(VAR d: T; d1, d2: T);
BEGIN
  FromDays(d, Days(d1) - Days(d2));
END Sub;

PROCEDURE Error(): CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c
END Error;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** Date.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    noRoomError       : Wr.Str('No room in destination string') |
    conversionError   : Wr.Str('Conversion error') |
    wrongDateError    : Wr.Str('Wrong date') |
    priviledgeError   : Wr.Str('Not enough privilegdes to get/set time')
  END;
  HALT;
END PrintError;

BEGIN
  errorCode := 0;
  errorHandler := PrintError;
END Date.