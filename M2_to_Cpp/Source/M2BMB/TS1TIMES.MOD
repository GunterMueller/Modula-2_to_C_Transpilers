(* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'Time' module. It contains operations for getting
    and setting the system time. Operations are characteristic for
    80x86/MS-DOS platform and means for doing them in
    Top Speed Modula-2 ver. 1.x.

    by Zoran Budimac, July, 1996
    ------------------------------------------------------------------ *)
IMPLEMENTATION MODULE TS1Times;
IMPORT SYSTEM, Lib, Time;

PROCEDURE SetST(t: Time.T; VAR ok: BOOLEAN);
VAR r: SYSTEM.Registers;
BEGIN
  r.AH := 02DH;
  r.CH := BYTE(t.hour);
  r.CL := BYTE(t.min);
  r.DH := BYTE(t.sec);
  r.DL := BYTE(t.hun);
  Lib.Dos(r);
  ok := TRUE
END SetST;

PROCEDURE GetST(VAR t: Time.T; VAR ok: BOOLEAN);
VAR r: SYSTEM.Registers;
BEGIN
  r.AH := 02CH;
  Lib.Dos(r);
  t.hour := CARDINAL(r.CH);
  t.min := CARDINAL(r.CL);
  t.sec := CARDINAL(r.DH);
  t.hun := CARDINAL(r.DL);
  ok := TRUE
END GetST;

END TS1Times.
