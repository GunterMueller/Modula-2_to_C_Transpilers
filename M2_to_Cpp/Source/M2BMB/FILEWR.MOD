(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       FileWr (1.1)                              *
 * Version:                 1.0                                       *
 * Date:                    February 27, 1996                         *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                Generic Modula-2 Compiler                 *
 * Portability:             This module is portable                   *
 *                                                                    *
 * Author:                      Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE FileWr;
IMPORT File;                (* BMB file operations *)
IMPORT Fmt;                 (* BMB string conversion operations *)

TYPE
  St = ARRAY [0..31] OF CHAR;

PROCEDURE Bool(f: File.T; b: BOOLEAN; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.Bool(s, b);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END Bool;

PROCEDURE Int(f: File.T; n: INTEGER; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.Int(s, n);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END Int;

PROCEDURE LongInt(f: File.T; n: LONGINT; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.LongInt(s, n);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END LongInt;

PROCEDURE Card(f: File.T; n: CARDINAL; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.Card(s, n);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END Card;

PROCEDURE LongCard(f: File.T; n: LONGCARD; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.LongCard(s, n);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END LongCard;

PROCEDURE Real(f: File.T; n: REAL; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.Real(s, n);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END Real;

PROCEDURE LongReal(f: File.T; n: LONGREAL; field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.LongReal(s, n);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END LongReal;

PROCEDURE IntB(f: File.T; n: INTEGER; base, field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.IntB(s, n, base);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END IntB;

PROCEDURE LongIntB(f: File.T; n: LONGINT; base, field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.LongIntB(s, n, base);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END LongIntB;

PROCEDURE CardB(f: File.T; n: CARDINAL; base, field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.CardB(s, n, base);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END CardB;

PROCEDURE LongCardB(f: File.T; n: LONGCARD; base, field: CARDINAL);
VAR s: St;
BEGIN
  Fmt.LongCardB(s, n, base);
  IF field <> 0 THEN Fmt.PadR(s, s, field, ' ') END;
  File.WrStr(f, s);
END LongCardB;

PROCEDURE Bin(f: File.T; bin: ARRAY OF BYTE);
BEGIN
  File.WrBin(f, bin, LONGCARD(SIZE(bin)));
END Bin;

END FileWr.
