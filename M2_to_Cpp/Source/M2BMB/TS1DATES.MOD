(* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'Date' module. It contains operations for getting
    and setting the system date. Operations are characteristic for
    80x86/MS-DOS platform and means for doing them in
    Top Speed Modula-2 ver. 1.x.

    by Zoran Budimac, November, 1996
    ------------------------------------------------------------------ *)
IMPLEMENTATION MODULE TS1Dates;
IMPORT SYSTEM, Lib, Date;

PROCEDURE SetSD(d: Date.T; VAR ok: BOOLEAN);
VAR r: SYSTEM.Registers;
BEGIN
  r.AH := 02BH;
  r.CX := WORD(d.year);
  r.DH := BYTE(d.month);
  r.DL := BYTE(d.day);
  Lib.Dos(r);
END SetSD;

PROCEDURE GetSD(VAR d: Date.T; VAR ok: BOOLEAN);
VAR r: SYSTEM.Registers;
BEGIN
  r.AH := 02AH;
  Lib.Dos(r);
  d.year := CARDINAL(r.CX);
  d.month := CARDINAL(r.DH);
  d.day := CARDINAL(r.DL);
END GetSD;

END TS1Dates.



