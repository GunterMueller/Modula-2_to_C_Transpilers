(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Scan (1.1)                                *
 *                          (See Scan.DOC for full specification)     *
 * Version:                 1.0                                       *
 * Date:                    July 15, 1996                             *
 * Conformance Test Passed: July 15, 1996                             *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * To do:                   Replace the current implementation of     *
 *                          'LongReal' with the generic one (if       *
 *                          conforms to BMB Conformance test)         *
 *                                                                    *
 * Author:                      Tamas Cservenak & Zoran Budimac       *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: {icervenak,zjb}@unsim.ns.ac.yu*
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Scan;
IMPORT String, Wr;
FROM GenericRealConvs IMPORT StrToReal;
     (* Generic and highly portable version *)
FROM TS1RealConvs     IMPORT StrToLongReal;
     (* Version closely tied to TopSpeed M2 Compiler - should be replaced
        soon *)

CONST
    minBase = 2;
    maxBase = 16;

VAR errorCode:  CARDINAL;
    digitChars: ARRAY [0..15] OF CHAR;  (* = "0123456789ABCDEF" *)

PROCEDURE StrToWhole(VAR num: LONGCARD; base: Base; str: ARRAY OF CHAR;
                     VAR done: BOOLEAN; VAR overflow: BOOLEAN);
  (* Converts 'str' to 'num' using 'base'. 'done' is TRUE if
     everything went OK. 'overflow' is TRUE if overflow occured *)
  (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
  VAR
    i: CARDINAL;
    digit: CARDINAL;
    ch: CHAR;
BEGIN
  overflow := FALSE;
  IF String.Length(str) = 0 THEN
     done := FALSE;
     RETURN
  END;
  i   := 0;
  num := 0;
  LOOP
    IF i <= HIGH(str) THEN
       ch := CAP(str[i])
    ELSE
       ch := 0C
    END;
    IF ("0" <= ch) & (ch <= "9") THEN
       digit := ORD(ch) - ORD("0")
    ELSIF ("A" <= ch) & (ch <= "F") THEN
       digit := ORD(ch) - ORD("A") + 10
    ELSE
      EXIT
    END;
    INC(i);

    IF (digit < base) & (num <= (MAX(LONGCARD) - LONGCARD(digit)) DIV LONGCARD(base)) THEN
      num := num * LONGCARD(base) + LONGCARD(digit);
    ELSIF (digit >= base)  THEN (* bad digit! *)
      EXIT
    ELSE                        (* overflow! *)
      overflow := TRUE;
      EXIT
    END;
  END;
  done := ch = 0C;
END StrToWhole;

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg);
END SetError;

PROCEDURE Bool(s: ARRAY OF CHAR): BOOLEAN;
   (* by Zoran Budimac, July, 1995 *)
VAR
  pos: CARDINAL;
BEGIN
  IF String.SubString(s, 'TRUE', pos) AND (pos = 0) THEN
     RETURN TRUE
  ELSIF String.SubString(s, 'FALSE', pos) AND (pos = 0) THEN
     RETURN FALSE
  ELSE
     SetError(conversionError, "Bool");
     RETURN FALSE
  END
END Bool;

PROCEDURE Int(s: ARRAY OF CHAR): INTEGER;
BEGIN
  RETURN IntB(s, 10)
END Int;

PROCEDURE LongInt(s: ARRAY OF CHAR): LONGINT;
BEGIN
  RETURN LongIntB(s, 10)
END LongInt;

PROCEDURE Card(s: ARRAY OF CHAR): CARDINAL;
BEGIN
  RETURN CardB(s, 10)
END Card;

PROCEDURE LongCard(s: ARRAY OF CHAR): LONGCARD;
BEGIN
  RETURN LongCardB(s, 10)
END LongCard;

PROCEDURE Real(s: ARRAY OF CHAR): REAL;
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR r: REAL;
    ok, over: BOOLEAN;
BEGIN
   r := StrToReal(s, ok, over);
   IF NOT ok THEN
      SetError(conversionError, "Real")
   END;
   IF over THEN
      SetError(overflowError, "Real");
   END;
   RETURN r
END Real;

PROCEDURE LongReal(s: ARRAY OF CHAR): LONGREAL;
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR r: LONGREAL;
    ok, over: BOOLEAN;
BEGIN
   r := StrToLongReal(s, ok, over);
   IF NOT ok THEN
      SetError(conversionError, "Real")
   END;
   IF over THEN
      SetError(overflowError, "Real");
   END;
   RETURN r
END LongReal;

PROCEDURE IntB(s: ARRAY OF CHAR; base: Base): INTEGER;
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR n: LONGCARD;
    ok,over,neg: BOOLEAN;
    p : CARDINAL;
BEGIN
   neg := String.SubString(s, '-', p);
   IF neg THEN String.Delete(s,p,p) END;
   StrToWhole(n, base, s, ok, over);
   (* The order is important! *)
   IF over OR
      neg  & (n > MAX(INTEGER)+1) OR
      ~neg & (n > MAX(INTEGER)) THEN
      SetError(overflowError, "IntB")
   END;
   IF (NOT ok) & (NOT over) THEN
      SetError(conversionError, "IntB")
   END;
   IF (base < minBase) OR (base > maxBase) THEN
      SetError(wrongBaseError, "IntB")
   END;
   IF neg THEN
     RETURN -1 * INTEGER(n)
   ELSE
     RETURN INTEGER(n)
   END
END IntB;

PROCEDURE LongIntB(s: ARRAY OF CHAR; base: Base): LONGINT;
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR n: LONGCARD;
    ok,neg,over: BOOLEAN;
    p : CARDINAL;
BEGIN
   neg := String.SubString(s,'-',p);
   IF neg THEN String.Delete(s,p,p) END;
   StrToWhole(n, base, s, ok, over);
   IF over OR
      neg  & (n > MAX(LONGINT)+1) OR
      ~neg & (n > MAX(LONGINT)) THEN
      SetError(overflowError, "LongInt")
   END;
   IF (NOT ok) & (NOT over) THEN
      SetError(conversionError, "LongInt")
   END;
   IF (base < minBase) OR (base > maxBase) THEN
      SetError(wrongBaseError, "LongIntB")
   END;
   IF neg THEN
     RETURN -1 * LONGINT(n)
   ELSE
     RETURN LONGINT(n)
   END
END LongIntB;

PROCEDURE CardB(s: ARRAY OF CHAR; base: Base): CARDINAL;
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR n: LONGCARD;
    ok,over: BOOLEAN;
BEGIN
   StrToWhole(n, base, s, ok,over);
   (* Errors. The order is important! *)
   IF over OR (n > MAX(CARDINAL)) THEN
      SetError(overflowError, "Card")
   END;
   IF (NOT ok) & (NOT over) THEN
      SetError(conversionError, "Card")
   END;
   IF (base < minBase) OR (base > maxBase) THEN
      SetError(wrongBaseError, "LongIntB")
   END;
   RETURN CARDINAL(n);
END CardB;

PROCEDURE LongCardB(s: ARRAY OF CHAR; base: Base): LONGCARD;
   (* by Tamas Cservenak, May, 1996. Modified by Zoran Budimac, July, 1996 *)
VAR n: LONGCARD;
    ok, over: BOOLEAN;
BEGIN
   StrToWhole(n, base, s, ok, over);
   (* Errors. The order is important *)
   IF over THEN
      SetError(overflowError, "LongCard")
   END;
   IF (NOT ok) & (NOT over) THEN
      SetError(conversionError, "LongCard")
   END;
   IF (base < minBase) OR (base > maxBase) THEN
      SetError(wrongBaseError, "LongIntB")
   END;
   RETURN n
END LongCardB;

PROCEDURE Error(): CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c
END Error;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** Scan.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    conversionError   : Wr.Str('Conversion error') |
    overflowError     : Wr.Str('Overflow error') |
    wrongBaseError    : Wr.Str('Wrong base')
  END;
  HALT;
END PrintError;

BEGIN
  errorCode := noError;
  digitChars := "0123456789ABCDEF";
  errorHandler := PrintError;
END Scan.