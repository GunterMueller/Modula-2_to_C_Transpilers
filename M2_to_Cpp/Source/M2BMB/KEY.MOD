(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Key (1.1)                                 *
 * Version:                 1.0                                       *
 * Date:                    March, 1996                               *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * Portability:             Non-portable operations.                  *
 *                                                                    *
 * Author:                      Bojan Sxkaljak                        *
 *                              University of Novi Sad, Yugoslavia    *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Key;

FROM Lib IMPORT Intr,Move,WordFill;
FROM SYSTEM IMPORT Registers,Ofs,Seg;
IMPORT Wr;

VAR
   qualifiersStatus:BITSET;
   errorCode: CARDINAL;

(*=========================================================================*)
(* Procedures for error handling.                                          *)
(*=========================================================================*)

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg)
END SetError;

PROCEDURE Error() : CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c;
END Error;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** Keyboard.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError    : Wr.Str('Unknown error');
  END;
  HALT;
END PrintError;

(*=========================================================================*)
(* Gets current state of qualifiers no meter if any other key is pressed.  *)
(* Procedure is realized by reading the BIOS DATA area, locations          *)
(* [0000:0417H] & [0000:0418H].                                            *)
(*=========================================================================*)

PROCEDURE Qualifiers():QualType;
VAR
  scanCode:CARDINAL;
BEGIN
    qualifiersStatus:=BITSET([0000:0417H]^);
    scanCode:=0;
    IF (BITSET{0} * qualifiersStatus <> BITSET{}) OR
       (BITSET{1} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierShift;       (* SHIFT is pressed *)
    END;
    IF (BITSET{2} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierCtrl;        (* CONTROL is pressed *)
    END;
    IF (BITSET{3} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierAlt;         (* ALT is pressed *)
    END;
    IF (BITSET{10} * qualifiersStatus <> BITSET{}) THEN
           scanCode:=scanCode+qualifierSysReq;     (* SysRq is pressed *)
    END;
    IF (BITSET{11} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierPause;       (* Pause is pressed *)
    END;
    IF (BITSET{12} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierScrollLock;  (* Scroll Lock is pressed *)
    END;
    IF (BITSET{13} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierNumLock;     (* Num Lock is pressed *)
    END;
    IF (BITSET{14} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierCapsLock;    (* Caps Lock is pressed *)
    END;
    IF (BITSET{15} * qualifiersStatus <> BITSET{}) THEN
          scanCode:=scanCode+qualifierInsert;      (* Insert is pressed *)
    END;
    RETURN scanCode;
END Qualifiers;

(*=========================================================================*)
(* Gets qualifier of last pressed key.                                     *)
(*=========================================================================*)

PROCEDURE LastQualifier():QualType;
VAR
  Status,CurrentState:BITSET;
  scanCode:CARDINAL;
BEGIN
    Status:=BITSET([0000:0417H]^);
    scanCode:=0;
    IF Status<>qualifiersStatus THEN
        (* only if state of qualifiers have changed *)
       CurrentState:=Status-qualifiersStatus;
       IF (BITSET{0} * CurrentState <> BITSET{}) OR (BITSET{1} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierShift;        (* SHIFT is pressed *)
       END;
       IF (BITSET{2} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierCtrl;         (* CONTROL is pressed *)
       END;
       IF (BITSET{3} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierAlt;          (* ALT is pressed *)
       END;
       IF (BITSET{10} * CurrentState <> BITSET{}) THEN
           scanCode:=scanCode+qualifierSysReq;      (* SysRq is pressed *)
       END;
       IF (BITSET{11} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierPause;        (* Pause is pressed *)
       END;
       IF (BITSET{12} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierScrollLock;   (* Scroll Lock is pressed *)
       END;
       IF (BITSET{13} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierNumLock;      (* Num Lock is pressed *)
       END;
       IF (BITSET{14} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierCapsLock;     (* Caps Lock is pressed *)
       END;
       IF (BITSET{15} * CurrentState <> BITSET{}) THEN
          scanCode:=scanCode+qualifierInsert;       (* Insert is pressed *)
       END;
       qualifiersStatus:=Status;
    END;
    RETURN scanCode;
END LastQualifier;

(*============================================================================*)
(* This procedure does scanning of the keyboard for pressed key and returns   *)
(* appropriate value as predefined constant according to the pressed key.    *)
(* If key is pressed, BIOS keyboard service 16H, subfunction 0 is used to     *)
(* pop key from buffer.Procedure is no echo.                                  *)
(*============================================================================*)

PROCEDURE Rd(): KeyType;
   (* Modified by Zoran Budimac, January, 1997 *)
   (* Now this function tries to read the key without checking for the
      pressed key first *)
VAR
  r:Registers;
  scanCode,qualifier:CARDINAL;
BEGIN
    scanCode:=0;
    r.AH:=00H;
    Intr(r,16H);
    scanCode:=CARDINAL(r.AH);
    IF scanCode>=f1+increaseShift THEN  (* functional keys have *)
       qualifier := Qualifiers();         (* diferent scan code when *)
       IF qualifier=qualifierShift THEN    (* they are pressed with *)
          DEC(scanCode,increaseShift);     (* qualifiers *)
       ELSIF qualifier=qualifierCtrl THEN
          DEC(scanCode,increaseCtrl);
       ELSIF (qualifier=qualifierAlt) OR (qualifier=qualifierCtrlAlt) OR
             (qualifier=qualifierAltShift) THEN
          DEC(scanCode,increaseAlt);
       ELSIF qualifier=qualifierCtrlShift THEN
          DEC(scanCode,increaseCtrl);
       END;
    END;
    RETURN scanCode;
END Rd;

PROCEDURE RdChar(): CHAR;
   (* Returns the character of the pressed key *)
   (* by Zoran Budimac, january, 1997 *)
VAR r: Registers;
BEGIN
  r.AH:=00H;
  Intr(r,16H);         (* returns ASCII code of pressed key in AL *)
  RETURN CHAR(r.AL);
END RdChar;


(*=========================================================================*)
(* Gets keyboard key-pressed status by simply comparing pointer to the     *)
(* start and pointer to the end of keyboard buffer.                        *)
(*=========================================================================*)

PROCEDURE Pressed():BOOLEAN;
   (* Daniel: Redefinisati~. Koristiti BIOS! *)
BEGIN
    RETURN ([0000:041AH]^<>[0000:041CH]^);
END Pressed;

(*===========================  SETTING DEFAULTS  =============================*)

BEGIN
    qualifiersStatus:=BITSET([0000:0417H]^);
END Key.
