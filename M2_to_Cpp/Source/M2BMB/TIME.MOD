(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Time (1.1)                                *
 * Version:                 1.0                                       *
 * Date:                    July 15, 1996                             *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * Remarks:                 Non-portable operations are contained in  *
 *                          separate module (TS1Times). To port this  *
 *                          module, provide XXXTimes module for XXX   *
 *                          platform.
 *                                                                    *
 * Author:                      Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Time;
IMPORT Fmt, Scan, String, Wr;
FROM TS1Times IMPORT SetST, GetST;
              (* Operations characteristic for Top Speed M2/80x86/MS-DOS *)
VAR
   errorCode : CARDINAL;

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg);
END SetError;

PROCEDURE OK(t: T): BOOLEAN;
   (* Returns TRUE if time 't' is a valid time *)
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN (Hour(t) >= 0) AND (Hour(t) <= 23) AND
         (Min(t) >= 0) AND (Min(t) <= 59) AND
         (Sec(t) >= 0) AND (Sec(t) <= 59) AND
         (Hund(t) >= 0) AND (Hund(t) <= 99)
END OK;

PROCEDURE Hund(t: T): CARDINAL;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN t.hun
END Hund;

PROCEDURE Sec(t: T): CARDINAL;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN t.sec
END Sec;

PROCEDURE Min(t: T): CARDINAL;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN t.min
END Min;

PROCEDURE Hour(t: T): CARDINAL;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN t.hour
END Hour;

PROCEDURE Init(VAR t: T; hh, mm, ss, hd: CARDINAL);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  t.hun := hh;
  t.sec := mm;
  t.min := ss;
  t.hour:= hd;
  IF NOT OK(t) THEN SetError(wrongTimeError, "Init") END;
END Init;

PROCEDURE Copy(VAR t: T; t1: T);
   (* by Zoran Budimac, July, 1996 *)
BEGIN
  t := t1
END Copy;

PROCEDURE SetHund(VAR t: T; n: CARDINAL);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  IF n>99 THEN SetError(wrongTimeError, "SetHund") END;
  t.hun := n
END SetHund;

PROCEDURE SetSec(VAR t: T; n: CARDINAL);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  IF n>59 THEN SetError(wrongTimeError, "SetSec") END;
  t.sec := n
END SetSec;

PROCEDURE SetMin(VAR t: T; n: CARDINAL);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  IF n>59 THEN SetError(wrongTimeError, "SetMin") END;
  t.min := n
END SetMin;

PROCEDURE SetHour(VAR t: T; n: CARDINAL);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  IF n>23 THEN SetError(wrongTimeError, "SetHour") END;
  t.hour := n
END SetHour;

PROCEDURE SetSystemTime(t: T);
   (* by Zoran Budimac, July, 1996 *)
VAR ok: BOOLEAN;
BEGIN
  SetST(t, ok)
END SetSystemTime;

PROCEDURE GetSystemTime(VAR t: T);
   (* by Zoran Budimac, July, 1996 *)
VAR ok: BOOLEAN;
BEGIN
  GetST(t, ok);
END GetSystemTime;

PROCEDURE ToString(VAR str: ARRAY OF CHAR; t: T);
   (* by Zoran Budimac, February, 1995 *)
VAR tmp: ARRAY [0..20] OF CHAR;
BEGIN
   (* Errors. The order is important *)
   IF 10 > HIGH(str) THEN SetError(noRoomError, "ToString") END;
   Fmt.Card(tmp, t.hour);
   Fmt.PadR(tmp, tmp, 2, '0');
   String.Concat(str, tmp, ":");
   Fmt.Card(tmp, t.min);
   Fmt.PadR(tmp, tmp, 2, '0');
   String.Append(str, tmp);
   String.Append(str, ":");
   Fmt.Card(tmp, t.sec);
   Fmt.PadR(tmp, tmp, 2, '0');
   String.Append(str, tmp);
   String.Append(str, '.');
   Fmt.Card(tmp, t.hun);
   Fmt.PadR(tmp, tmp, 2, '0');
   String.Append(str, tmp);
END ToString;

PROCEDURE FromString(VAR t: T; str: ARRAY OF CHAR);
   (* by Zoran Budimac, February, 1995 *)
VAR tmp : ARRAY [0..10] OF CHAR;
    pos, n: CARDINAL;
BEGIN
  Init(t, 0, 0, 0, 0);

  (* Hours *)
  IF NOT String.SubString(str, ":", pos) THEN
     SetError(conversionError, "FromString")
  END;
  String.Slice(tmp, str, 0, pos);
  n := Scan.Card(tmp);
  SetHour(t, n);
  String.Delete(str, 0, pos+1);

  (* Minutes *)
  IF NOT String.SubString(str, ":", pos) THEN
     (* String was in the form 'hh:mm' *)
     RETURN
  END;
  String.Slice(tmp, str, 0, pos);
  n := Scan.Card(tmp);
  SetMin(t, n);
  String.Delete(str, 0, pos+1);

  (* Seconds *)
  IF NOT String.SubString(str, ".", pos) THEN
     (* String was in the form 'hh:mm' *)
     RETURN
  END;
  String.Slice(tmp, str, 0, pos);
  n := Scan.Card(tmp);
  SetSec(t, n);
  String.Delete(str, 0, pos+1);

  (* Hundreds *)
  n := Scan.Card(str);
  SetHund(t, n);
  IF NOT OK(t) THEN SetError(wrongTimeError, "FromString") END;
END FromString;

PROCEDURE Hundts(t: T): LONGCARD;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  RETURN 360000*LONGCARD(t.hour) + 6000*LONGCARD(t.min) +
         100*LONGCARD(t.sec) + LONGCARD(t.hun)
END Hundts;

PROCEDURE FromHundts(VAR t: T; hundts: LONGCARD);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  t.hun  := CARDINAL(hundts MOD 100);
  hundts := hundts DIV 100;
  t.sec  := CARDINAL(hundts MOD 60);
  hundts := hundts DIV 60;
  t.min  := CARDINAL(hundts MOD 60);
  hundts := hundts DIV 60;
  t.hour := CARDINAL(hundts MOD 60);
  IF NOT OK(t) THEN SetError(wrongTimeError, "FromHundts") END;
END FromHundts;

PROCEDURE Inc(VAR t: T; hun: LONGCARD);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  FromHundts(t, Hundts(t) + hun);
END Inc;

PROCEDURE Dec(VAR t: T; hun: LONGCARD);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  FromHundts(t, Hundts(t) - hun);
END Dec;

PROCEDURE Add(VAR t: T; t1, t2: T);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  FromHundts(t, Hundts(t1) + Hundts(t2));
END Add;

PROCEDURE Sub(VAR t: T; t1, t2: T);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  FromHundts(t, Hundts(t1) - Hundts(t2));
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
  Wr.Str('*** Time.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    noRoomError       : Wr.Str('No room in destination string') |
    conversionError   : Wr.Str('Conversion error') |
    wrongTimeError    : Wr.Str('Wrong time') |
    priviledgeError   : Wr.Str('Not enough privilegdes to get/set time')
  END;
  HALT;
END PrintError;

BEGIN
  errorCode := 0;
  errorHandler := PrintError;
END Time.