(* ---------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library               *
 *                                                                  *
 * Implementation of:     Rd (1.00)                                 *
 * Version:               1.0                                       *
 * Date:                  March, 1995                               *
 * Platform:              80x86 / MS-DOS                            *
 * Requires:              TopSpeed Modula-2 ver. 1.x                *
 *                                                                  *
 * Author:                    Zoran Budimac                         *
 *                            University of Novi Sad, Yugoslavia    *
 *                            e-mail: zjb@unsim.ns.ac.yu            *
 * ---------------------------------------------------------------- *)

IMPLEMENTATION MODULE Rd;
IMPORT File; IMPORT FileRd;

PROCEDURE Bool(): BOOLEAN;
BEGIN
  RETURN FileRd.Bool(File.StdIn())
END Bool;

PROCEDURE Int(): INTEGER;
BEGIN
  RETURN FileRd.Int(File.StdIn())
END Int;

PROCEDURE LongInt(): LONGINT;
BEGIN
  RETURN FileRd.LongInt(File.StdIn())
END LongInt;

PROCEDURE Card(): CARDINAL;
BEGIN
  RETURN FileRd.Card(File.StdIn())
END Card;

PROCEDURE LongCard(): LONGCARD;
BEGIN
  RETURN FileRd.LongCard(File.StdIn())
END LongCard;

PROCEDURE Real(): REAL;
BEGIN
  RETURN FileRd.Real(File.StdIn())
END Real;

PROCEDURE LongReal(): LONGREAL;
BEGIN
  RETURN FileRd.LongReal(File.StdIn())
END LongReal;

PROCEDURE IntB(base: CARDINAL): INTEGER;
BEGIN
  RETURN FileRd.IntB(File.StdIn(), base)
END IntB;

PROCEDURE LongIntB(base: CARDINAL): LONGINT;
BEGIN
  RETURN FileRd.LongIntB(File.StdIn(), base)
END LongIntB;

PROCEDURE CardB(base: CARDINAL): CARDINAL;
BEGIN
  RETURN FileRd.CardB(File.StdIn(), base)
END CardB;

PROCEDURE LongCardB(base: CARDINAL): LONGCARD;
BEGIN
  RETURN FileRd.LongCardB(File.StdIn(), base)
END LongCardB;

PROCEDURE Char(): CHAR;
BEGIN
  RETURN FileRd.Char(File.StdIn())
END Char;

PROCEDURE Line(VAR line: ARRAY OF CHAR);
BEGIN
  FileRd.Line(File.StdIn(), line)
END Line;

PROCEDURE Ln();
BEGIN
  FileRd.Ln(File.StdIn())
END Ln;

PROCEDURE Bin(VAR bin: ARRAY OF BYTE);
BEGIN
  FileRd.Bin(File.StdIn(), bin)
END Bin;

PROCEDURE Error(): CARDINAL;
BEGIN
  RETURN FileRd.Error()
END Error;

END Rd.
