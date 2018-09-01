(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Fmt (1.10)                                *
 * Version:                 1.0                                       *
 * Date:                    July 15, 1996                             *
 * Conformance Test Passed: July 15, 1996                             *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * To do:                   Replace the current implementation of     *
 *                          'Real' and 'LongReal' from TS1RealConvs   *
 *                          with the more general one - perhaps with  *
 *                          the one that is related to the            *
 *                          representation of reals and not to the    *
 *                          Modula-2 compiler                         *
 * Portability:             Non-portable operations are contained in  *
 *                          separate module (TS1RealConvs). To port   *
 *                          this module, provide XXXRealConvs module  *
 *                          for XXX platform.                         *
 *                                                                    *
 * Author:                      Tamas Cservenak & Zoran Budimac       *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: {icervenak,zjb}@unsim.ns.ac.yu*
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Fmt;
IMPORT String, Wr;
FROM TS1RealConvs IMPORT RealToStr, LongRealToStr;
     (* Specific real conversions for TopSpeed M2 compiler ver. 1.xx
        and DOS/80x86 *)

CONST
    minBase = 2;
    maxBase = 16;

VAR errorCode:  CARDINAL;
    digitChars: ARRAY [0..15] OF CHAR;  (* = "0123456789ABCDEF" *)

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg);
END SetError;

PROCEDURE WholeToStr(num: LONGCARD; base: Base; VAR str: ARRAY OF CHAR;
                     VAR done: BOOLEAN);
  (* Convert 'num' to 'str' using 'base' *)
  VAR i,j: CARDINAL;
BEGIN
  done := TRUE;
  i := HIGH(str)+1;
  REPEAT  (* str[i..HIGH(str)] contains conversion result *)
    IF i <= 0 THEN  (* error: num does not fit into str *)
      done := FALSE;
      RETURN
    END;
    DEC(i);
    str[i] := digitChars[CARDINAL(num MOD LONGCARD(base))];
    num := num DIV LONGCARD(base);
  UNTIL num = 0;

  (* copy conversion result from str[i..HIGH(str)] to str[0..] *)
  j := 0;
  WHILE i <= HIGH(str) DO
    str[j] := str[i];
    INC(i);
    INC(j)
  END;
  IF j <= HIGH(str) THEN  (* make ASCIIZ *)
    str[j] := 0C
  END
END WholeToStr;

PROCEDURE Bool(VAR s: ARRAY OF CHAR; b: BOOLEAN);
   (* by Zoran Budimac, February, 1995 *)
BEGIN
  IF b & (HIGH(s)>=3) THEN
     String.Copy(s, 'TRUE');
  ELSIF ~b & (HIGH(s)>=4) THEN
     String.Copy(s, 'FALSE');
  ELSE
     SetError(noRoomError, "Bool")
  END;
END Bool;

PROCEDURE Int(VAR s: ARRAY OF CHAR;  n: INTEGER);
BEGIN
  IntB(s, n, 10)
END Int;

PROCEDURE LongInt(VAR s: ARRAY OF CHAR; n: LONGINT);
BEGIN
  LongIntB(s, n, 10)
END LongInt;

PROCEDURE Card(VAR s: ARRAY OF CHAR; n: CARDINAL);
BEGIN
  CardB(s, n, 10)
END Card;

PROCEDURE LongCard(VAR s: ARRAY OF CHAR; n: LONGCARD);
BEGIN
  LongCardB(s, n, 10)
END LongCard;

PROCEDURE Real(VAR s: ARRAY OF CHAR; x: REAL);
   (* by Zoran Budimac, July 11, 1995 *)
VAR ok: BOOLEAN;
    i, posE: CARDINAL;
    str: ARRAY [0..200] OF CHAR;  (* RealToStr generates intermediate
                                     string that can be biger than the
                                     final one *)
BEGIN
  RealToStr(str, x, ok);
  IF NOT ok THEN SetError(noRoomError, "Real") END;
  (* Since 'str' still does not conform to the BMB requirements,
     some adjustments are in order *)
  (* Trim from the left *)
  i := 0;
  WHILE str[i] = " " DO
     String.Delete(str, i, i);
     INC(i)
  END;
  (* Left out all consequtive zeros in decimal places *)
  IF String.SubString(str, "E", posE) THEN
     i := posE-1;
     WHILE str[i] = '0' DO
        String.Delete(str, i, i);
        DEC(i)
     END;
  END;
  IF String.Length(str) > HIGH(s)+1 THEN
     SetError(noRoomError, "Real")
  ELSE
     String.Copy(s, str)
  END
END Real;

PROCEDURE LongReal(VAR s: ARRAY OF CHAR; x: LONGREAL);
   (* by Zoran Budimac, July 11, 1995 *)
VAR ok: BOOLEAN;
    i, posE: CARDINAL;
    str: ARRAY [0..200] OF CHAR;  (* RealToStr generates intermediate
                                     string that can be biger than the
                                     final one *)
BEGIN
  LongRealToStr(str, x, ok);
  IF NOT ok THEN SetError(noRoomError, "Real") END;
  (* Since 's' still does not conform to the BMB requirements,
     some adjustments are in order *)
  (* Trim from the left *)
  i := 0;
  WHILE str[i] = " " DO
     String.Delete(str, i, i);
     INC(i)
  END;
  (* Left out all consequtive zeros in decimal places *)
  IF String.SubString(str, "E", posE) THEN
     i := posE-1;
     WHILE str[i] = '0' DO
        String.Delete(str, i, i);
        DEC(i)
     END;
  END;
  IF String.Length(str) > HIGH(s)+1 THEN
     SetError(noRoomError, "Real")
  ELSE
     String.Copy(s, str)
  END
END LongReal;

PROCEDURE IntB(VAR s: ARRAY OF CHAR; n: INTEGER; base: Base);
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR neg, ok: BOOLEAN;
BEGIN
  IF (base >= minBase) AND (base <= maxBase) THEN
     neg := n < 0;
     WholeToStr(LONGCARD(ABS(n)), base, s, ok);
     IF NOT ok THEN SetError(noRoomError, "IntB") END;
     IF neg THEN String.Insert(s, '-', 0) END
  ELSE
     SetError(wrongBaseError, "IntB")
  END
END IntB;

PROCEDURE LongIntB(VAR s: ARRAY OF CHAR; n: LONGINT; base: Base);
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR neg, ok: BOOLEAN;
BEGIN
  IF (base >= minBase) AND (base <= maxBase) THEN
     neg := n < 0;
     WholeToStr(LONGCARD(ABS(n)), base, s, ok);
     IF NOT ok THEN SetError(noRoomError, "LongIntB") END;
     IF neg THEN String.Insert(s, '-', 0) END
  ELSE
     SetError(wrongBaseError, "LongIntB")
  END
END LongIntB;

PROCEDURE CardB(VAR s: ARRAY OF CHAR; n: CARDINAL; base: Base);
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR ok: BOOLEAN;
BEGIN
  IF (base >= minBase) AND (base <= maxBase) THEN
     WholeToStr(LONGCARD(n), base, s, ok);
     IF NOT ok THEN SetError(noRoomError, "CardB") END;
  ELSE
     SetError(wrongBaseError, "LongIntB")
  END
END CardB;

PROCEDURE LongCardB(VAR s: ARRAY OF CHAR; n: LONGCARD; base: Base);
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR ok: BOOLEAN;
BEGIN
  IF (base >= minBase) AND (base <= maxBase) THEN
     WholeToStr(n, base, s, ok);
     IF NOT ok THEN SetError(noRoomError, "LomgCardB") END;
  ELSE
     SetError(wrongBaseError, "LongIntB")
  END
END LongCardB;

PROCEDURE Empty(VAR s: ARRAY OF CHAR; n: CARDINAL; ch: CHAR);
   (* 's' is a string consisting 'n' characters 'ch' *)
   (* by Zoran Budimac, February, 1995.
      Modified by Tamas Cservenak, May, 1996 *)
VAR i   : CARDINAL;
    str : ARRAY[0..0] OF CHAR; (* String to hold character 'ch' *)
BEGIN
   str[0] := ch;
   String.Copy(s, '');
   FOR i := 1 TO n DO
      String.Append(s, str)
   END;
END Empty;

PROCEDURE PadL(VAR s1: ARRAY OF CHAR; s: ARRAY OF CHAR;
               n: CARDINAL; ch: CHAR);
   (* by Zoran Budimac, February, 1995 *)
VAR l: CARDINAL;
    t: ARRAY [0..255] OF CHAR;
BEGIN
  l := String.Length(s);
  IF (l > n) OR (l > HIGH(s1)+1) OR (n > HIGH(s1)+1)  THEN
     errorCode := noRoomError
  ELSE
     Empty(t, n-l, ch);
     String.Concat(s1, s, t);
  END
END PadL;

PROCEDURE PadR(VAR s1: ARRAY OF CHAR; s: ARRAY OF CHAR;
               n: CARDINAL; ch: CHAR);
   (* by Zoran Budimac, February, 1995 *)
VAR l: CARDINAL;
    t: ARRAY [0..255] OF CHAR;
BEGIN
  errorCode := noError;
  l := String.Length(s);
  IF (l > n) OR (l > HIGH(s1)+1) OR (n > HIGH(s1)+1) THEN
     errorCode := noRoomError;
  ELSE
     Empty(t, n-l, ch);
     String.Concat(s1, t, s);
  END
END PadR;

PROCEDURE PadC(VAR s1: ARRAY OF CHAR; s: ARRAY OF CHAR;
               n: CARDINAL; ch: CHAR);
   (* by Zoran Budimac, February, 1995 *)
VAR l: CARDINAL;
    t: ARRAY [0..255] OF CHAR;
BEGIN
  errorCode := noError;
  l := String.Length(s);
  IF (l > n) OR (l > HIGH(s1)+1) OR (n > HIGH(s1)+1) THEN
     errorCode := noRoomError;
  ELSE
     Empty(t, (n-l) DIV 2, ch);
     String.Concat(s1, t, s);
     Empty(t, n - l - (n-l) DIV 2, ch);
     String.Concat(s1, s1, t);
  END
END PadC;

PROCEDURE Error(): CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c
END Error;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** Fmt.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    noRoomError       : Wr.Str('No room in destination string') |
    wrongBaseError    : Wr.Str('Wrong base')
  END;
  HALT;
END PrintError;

BEGIN
  errorCode := noError;
  digitChars := "0123456789ABCDEF";
  errorHandler := PrintError;
END Fmt.