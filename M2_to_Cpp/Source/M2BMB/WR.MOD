(* ---------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library               *
 *                                                                  *
 * Implementation of:     Wr (1.00)                                 *
 * Version:               1.0                                       *
 * Date:                  March, 1995                               *
 * Platform:              80x86 / MS-DOS                            *
 * Requires:              TopSpeed Modula-2 ver. 1.x                *
 *                                                                  *
 * Author:                    Zoran Budimac                         *
 *                            University of Novi Sad, Yugoslavia    *
 *                            e-mail: zjb@unsim.ns.ac.yu            *
 * ---------------------------------------------------------------- *)

IMPLEMENTATION MODULE Wr;
IMPORT File; IMPORT FileWr;

PROCEDURE Bool(b: BOOLEAN; field: CARDINAL);
BEGIN
  FileWr.Bool(File.StdOut(), b, field);
  File.Flush(File.StdOut())
END Bool;

PROCEDURE Int(n: INTEGER; field: CARDINAL);
BEGIN
  FileWr.Int(File.StdOut(), n, field);
  File.Flush(File.StdOut())
END Int;

PROCEDURE LongInt(n: LONGINT; field: CARDINAL);
BEGIN
  FileWr.LongInt(File.StdOut(), n, field);
  File.Flush(File.StdOut())
END LongInt;

PROCEDURE Card(n: CARDINAL; field: CARDINAL);
BEGIN
  FileWr.Card(File.StdOut(), n, field);
  File.Flush(File.StdOut())
END Card;

PROCEDURE LongCard(n: LONGCARD; field: CARDINAL);
BEGIN
  FileWr.LongCard(File.StdOut(), n, field);
  File.Flush(File.StdOut())
END LongCard;

PROCEDURE Real(n: REAL; field: CARDINAL);
BEGIN
  FileWr.Real(File.StdOut(), n, field);
  File.Flush(File.StdOut())
END Real;

PROCEDURE LongReal(n: LONGREAL; field: CARDINAL);
BEGIN
  FileWr.LongReal(File.StdOut(), n, field);
  File.Flush(File.StdOut())
END LongReal;

PROCEDURE IntB(n: INTEGER; base, field: CARDINAL);
BEGIN
  FileWr.IntB(File.StdOut(), n, base, field);
  File.Flush(File.StdOut())
END IntB;

PROCEDURE LongIntB(n: LONGINT; base, field: CARDINAL);
BEGIN
  FileWr.LongIntB(File.StdOut(), n, base, field);
  File.Flush(File.StdOut())
END LongIntB;

PROCEDURE CardB(n: CARDINAL; base, field: CARDINAL);
BEGIN
  FileWr.CardB(File.StdOut(), n, base, field);
  File.Flush(File.StdOut())
END CardB;

PROCEDURE LongCardB(n: LONGCARD; base, field: CARDINAL);
BEGIN
  FileWr.LongCardB(File.StdOut(), n, base, field);
  File.Flush(File.StdOut())
END LongCardB;

PROCEDURE Char(ch: CHAR);
BEGIN
  FileWr.Char(File.StdOut(), ch);
  File.Flush(File.StdOut())
END Char;

PROCEDURE Str(s: ARRAY OF CHAR);
BEGIN
  FileWr.Str(File.StdOut(), s);
  File.Flush(File.StdOut())
END Str;

PROCEDURE Ln();
BEGIN
  FileWr.Ln(File.StdOut());
  File.Flush(File.StdOut())
END Ln;

PROCEDURE Bin(bin: ARRAY OF BYTE);
BEGIN
  FileWr.Bin(File.StdOut(), bin);
  File.Flush(File.StdOut())
END Bin;

END Wr.
