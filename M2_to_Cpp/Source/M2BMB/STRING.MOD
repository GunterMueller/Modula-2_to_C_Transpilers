(* ---------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library               *
 *                                                                  *
 * Implementation of:     String (1.00)                             *
 * Version:               1.0                                       *
 * Date:                  February, 1995                            *
 * Platform:              80x86 / MS-DOS                            *
 * Requires:              TopSpeed Modula-2 ver. 3.x                *
 *                                                                  *
 * Author:                    Zoran Budimac                         *
 *                            University of Novi Sad, Yugoslavia    *
 *                            e-mail: zjb@unsim.ns.ac.yu            *
 * ---------------------------------------------------------------- *)

IMPLEMENTATION MODULE String;
IMPORT Str;

VAR ErrorCode: CARDINAL;

PROCEDURE Delete(VAR s: ARRAY OF CHAR; from, to: CARDINAL);
BEGIN
  IF to >= from THEN
    Str.Delete(s, from, to-from+1);
  END;
END Delete;

PROCEDURE Slice(VAR s: ARRAY OF CHAR; p: ARRAY OF CHAR; from, to: CARDINAL);
BEGIN
  IF to >= from THEN
    Str.Slice(s, p, from, to-from+1);
  END;
END Slice;

PROCEDURE SubString(s, r: ARRAY OF CHAR; VAR pos: CARDINAL): BOOLEAN;
BEGIN
  pos := Str.Pos(s, r);
  RETURN pos < MAX(CARDINAL);
END SubString;

PROCEDURE Equal(s, r: ARRAY OF CHAR): BOOLEAN;
BEGIN
  ErrorCode := 0;
  RETURN Str.Compare(s, r) = 0
END Equal;

PROCEDURE LEq(s, r: ARRAY OF CHAR): BOOLEAN;
BEGIN
  ErrorCode := 0;
  RETURN Str.Compare(s, r) <= 0
END LEq;

PROCEDURE GEq(s, r: ARRAY OF CHAR): BOOLEAN;
BEGIN
  ErrorCode := 0;
  RETURN Str.Compare(s, r) >= 0
END GEq;

PROCEDURE Error(): CARDINAL;
BEGIN
  RETURN ErrorCode;
END Error;

BEGIN
  ErrorCode := 0;
END String.