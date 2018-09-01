(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       FileRd (1.1)                              *
 * Version:                 1.0                                       *
 * Date:                    December 20, 1996                         *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                Generic Modula-2 Compiler                 *
 * Portability:             This module is portable                   *
 *                                                                    *
 * Author:                      Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE FileRd;
IMPORT File;                (* BMB file operations *)
IMPORT Scan;                (* BMB string conversion operations *)
IMPORT String;

TYPE
  St = ARRAY [0..31] OF CHAR;

PROCEDURE SkipSpaces(f: File.T; VAR ch: CHAR);
   (* Skips spaces in file 'f', returning the last non-space character
      in 'ch' *)
BEGIN
  ch := File.RdChar(f);
  WHILE (ch = ' ') OR (ch = 11C) OR (ch = 15C) OR (ch = 12C) DO
     ch := File.RdChar(f)
  END;
END SkipSpaces;

PROCEDURE Bool(f: File.T): BOOLEAN;
   (* by Zoran Budimac, December, 1996 *)
VAR s: St;
    b: BOOLEAN;
    ch: CHAR;
BEGIN
  SkipSpaces(f, ch);
  String.Copy(s, '');
  IF ch = 'T' THEN
     String.Append(s, ch);
     ch := File.RdChar(f);
     IF ch = 'R' THEN
        String.Append(s, ch);
        ch := File.RdChar(f);
        IF ch = 'U' THEN
           String.Append(s, ch);
           ch := File.RdChar(f);
           IF ch = 'E' THEN
              String.Append(s, ch);
              ch := File.RdChar(f);
           END
        END
     END
  ELSIF ch = 'F' THEN
     String.Append(s, ch);
     ch := File.RdChar(f);
     IF ch = 'A' THEN
        String.Append(s, ch);
        ch := File.RdChar(f);
        IF ch = 'L' THEN
           String.Append(s, ch);
           ch := File.RdChar(f);
           IF ch = 'S' THEN
              String.Append(s, ch);
              ch := File.RdChar(f);
              IF ch = 'E' THEN
                 String.Append(s, ch);
                 ch := File.RdChar(f);
              END
           END
        END
     END
  END;
  b := Scan.Bool(s);
  File.UnRdChar(f);
  RETURN b;
END Bool;

PROCEDURE Int(f: File.T): INTEGER;
   (* by Zoran Budimac, December, 1996 *)
VAR ch: CHAR;
    s: St;
    n: INTEGER;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  IF ch = '-' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
  END;
  WHILE (ch >= '0') OR (ch <= '9') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.Int(s);
  File.UnRdChar(f);
  RETURN n;
END Int;

PROCEDURE LongInt(f: File.T): LONGINT;
   (* by Zoran Budimac, December 1996 *)
VAR s: St;
    n: LONGINT;
    ch: CHAR;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  IF ch = '-' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
  END;
  WHILE (ch >= '0') OR (ch <= '9') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.LongInt(s);
  File.UnRdChar(f);
  RETURN n;
END LongInt;

PROCEDURE Card(f: File.T): CARDINAL;
   (* by Zoran Budimac, December 1996 *)
VAR s: St;
    n: CARDINAL;
    ch: CHAR;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  WHILE (ch >= '0') OR (ch <= '9') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.Card(s);
  File.UnRdChar(f);
  RETURN n;
END Card;

PROCEDURE LongCard(f: File.T): LONGCARD;
   (* by Zoran Budimac, December 1996 *)
VAR s: St;
    n: LONGCARD;
    ch: CHAR;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  WHILE (ch >= '0') OR (ch <= '9') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.LongCard(s);
  File.UnRdChar(f);
  RETURN n;
END LongCard;

PROCEDURE Real(f: File.T): REAL;
   (* by Zoran Budimac, December 1996 *)
VAR s: St;
    n: REAL;
    ch: CHAR;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  IF ch = '-' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
  END;
  WHILE (ch >= '0') OR (ch <= '9') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  IF ch = '.' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
     WHILE (ch >= '0') OR (ch <= '9') DO
        String.Append(s, ch);
        ch := File.RdChar(f);
     END
  END;
  IF CAP(ch) = 'E' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
     IF ch = '+' THEN
        String.Append(s, '-');
        ch := File.RdChar(f);
     ELSIF ch = '-' THEN
        String.Append(s, '-');
        ch := File.RdChar(f);
     END;
     WHILE (ch >= '0') OR (ch <= '9') DO
        String.Append(s, ch);
        ch := File.RdChar(f);
     END
  END;
  n := Scan.Real(s);
  File.UnRdChar(f);
  RETURN n;
END Real;

PROCEDURE LongReal(f: File.T): LONGREAL;
   (* by Zoran Budimac, December 1996 *)
VAR s: St;
    n: LONGREAL;
    ch: CHAR;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  IF ch = '-' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
  END;
  WHILE (ch >= '0') OR (ch <= '9') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  IF ch = '.' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
     WHILE (ch >= '0') OR (ch <= '9') DO
        String.Append(s, ch);
        ch := File.RdChar(f);
     END
  END;
  IF CAP(ch) = 'E' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
     IF ch = '+' THEN
        String.Append(s, '-');
        ch := File.RdChar(f);
     ELSIF ch = '-' THEN
        String.Append(s, '-');
        ch := File.RdChar(f);
     END;
     WHILE (ch >= '0') OR (ch <= '9') DO
        String.Append(s, ch);
        ch := File.RdChar(f);
     END
  END;
  n := Scan.LongReal(s);
  File.UnRdChar(f);
  RETURN n;
END LongReal;

PROCEDURE IntB(f: File.T; base: Scan.Base): INTEGER;
   (* by Zoran Budimac, December, 1996 *)
VAR ch: CHAR;
    s: St;
    n: INTEGER;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  IF ch = '-' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
  END;
  WHILE (ch >= '0') OR (ch <= '9') OR
        (CAP(ch) >= 'A') OR (CAP(ch) <= 'F') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.IntB(s, base);
  File.UnRdChar(f);
  RETURN n;
END IntB;

PROCEDURE LongIntB(f: File.T; base: Scan.Base): LONGINT;
   (* by Zoran Budimac, December, 1996 *)
VAR ch: CHAR;
    s: St;
    n: LONGINT;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  IF ch = '-' THEN
     String.Append(s, '-');
     ch := File.RdChar(f);
  END;
  WHILE (ch >= '0') OR (ch <= '9') OR
        (CAP(ch) >= 'A') OR (CAP(ch) <= 'F') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.LongIntB(s, base);
  File.UnRdChar(f);
  RETURN n;
END LongIntB;

PROCEDURE CardB(f: File.T; base: Scan.Base): CARDINAL;
   (* by Zoran Budimac, December, 1996 *)
VAR ch: CHAR;
    s: St;
    n: CARDINAL;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  WHILE (ch >= '0') OR (ch <= '9') OR
        (CAP(ch) >= 'A') OR (CAP(ch) <= 'F') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.CardB(s, base);
  File.UnRdChar(f);
  RETURN n;
END CardB;

PROCEDURE LongCardB(f: File.T; base: Scan.Base): LONGCARD;
   (* by Zoran Budimac, December, 1996 *)
VAR ch: CHAR;
    s: St;
    n: LONGCARD;
BEGIN
  String.Copy(s, '');
  SkipSpaces(f, ch);
  WHILE (ch >= '0') OR (ch <= '9') OR
        (CAP(ch) >= 'A') OR (CAP(ch) <= 'F') DO
     String.Append(s, ch);
     ch := File.RdChar(f);
  END;
  n := Scan.LongCardB(s, base);
  File.UnRdChar(f);
  RETURN n;
END LongCardB;

PROCEDURE Bin(f: File.T; VAR bin: ARRAY OF BYTE);
VAR dummy: LONGCARD;
BEGIN
  dummy := File.RdBin(f, bin, LONGCARD(SIZE(bin)))
END Bin;

END FileRd.
