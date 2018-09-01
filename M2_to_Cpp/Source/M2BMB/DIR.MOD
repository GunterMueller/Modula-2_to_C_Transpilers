(* ------------------------------------------------------------------ *
 *              BMB Modula-2 Cross-Platform Library                   *
 *                                                                    *
 * Implementation of:       Dir (1.1)                                 *
 * Version:                 1.0                                       *
 * Date:                    October 23, 1996                          *
 * Conformance Test Passed: -                                         *
 * Platform:                80x86 / MS-DOS                            *
 * Requires:                TopSpeed Modula-2 ver. 1.x                *
 * Acknowledgement:         Implementation based on TS FIO module     *
 *                                                                    *
 * Author:                  Daniel Sendula                            *
 *                          (for 'Black Mailbox' Group)               *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: isendula@unsim.ns.ac.yu       *
 * ------------------------------------------------------------------ *)

(*$S-,R-,I-*)
IMPLEMENTATION MODULE Dir;

FROM SYSTEM IMPORT Seg,Ofs,Registers,CarryFlag;
IMPORT AsmLib,Lib;

IMPORT FileName;   (* BMB Modules *)
IMPORT String, Wr;

CONST
  DiskIsFull = 0F0H ; (* Error if Write fails with disk full *)

(*$V-*)

TYPE
  FileInf   = POINTER TO BufRec;
  BufRec =  RECORD
              RWPos,
              EOB,
              BufSize : CARDINAL;
              Buffer  : ARRAY[0..0] OF SHORTCARD;
            END;
  PathStr  = ARRAY[0..64] OF CHAR;

VAR
  errorCode: CARDINAL;

(* ----------------------------------------------------------------- *)

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
  Wr.Str('*** Dir.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError     : Wr.Str('Unknown error');
  | diskFullError    : Wr.Str('Disk is full');
  | noRoomError      : Wr.Str('Buffer exceeded');
  | pathNotFoundError: Wr.Str('Path not found');
  | priviledgeError  : Wr.Str('Priviledge error');
  END;
  HALT;
END PrintError;

PROCEDURE ErrorCheck ( VAR r : Registers; msg: ARRAY OF CHAR);
VAR
  S:CARDINAL;
BEGIN
  WITH r DO
     IF (BITSET{CarryFlag} * Flags) # BITSET{} THEN
       S := AX;
       CASE S OF
       |  3 : SetError(pathNotFoundError, msg);
       |  5 : SetError(priviledgeError, msg);
       | DiskIsFull : SetError(diskFullError, msg);
       ELSE
         SetError(unknownError, msg);
       END;
     END;
  END;
END ErrorCheck;

(* ----------------------------------------------------------------- *)

PROCEDURE GetName(name: ARRAY OF CHAR; VAR fn: PathStr);
BEGIN
  String.Copy(fn,name);
  fn[SIZE(fn)-1] := CHR(0);
END GetName;

PROCEDURE ChangeTo(Name: ARRAY OF CHAR);
VAR
  r  : Registers;
  fn : PathStr;
BEGIN
  GetName(Name,fn);
  WITH r DO
    IF fn[1] = ':' THEN
      AH := 0EH;
      DL := SHORTCARD(CAP(fn[0]))-SHORTCARD('A');
      Lib.Dos(r);
    END;
    DS := Seg(fn);
    DX := Ofs(fn);
    AH := 3BH;
    Lib.Dos(r);
    ErrorCheck(r, 'ChangeTo');
  END;
END ChangeTo;

PROCEDURE Make(Name: ARRAY OF CHAR);
VAR
  r  : Registers;
  fn : PathStr;
BEGIN
  GetName(Name,fn);
  WITH r DO
    DS := Seg(fn);
    DX := Ofs(fn);
    AH := 39H;
    Lib.Dos(r);
    ErrorCheck(r, 'Make');
  END;
END Make;

PROCEDURE Remove(Name: ARRAY OF CHAR);
VAR
  r  : Registers;
  fn : PathStr;
BEGIN
  GetName(Name,fn);
  WITH r DO
    DS := Seg(fn);
    DX := Ofs(fn);
    AH := 3AH;
    Lib.Dos(r);
    ErrorCheck(r, 'Remove');
  END;
END Remove;

PROCEDURE GetCurrent(VAR Name: ARRAY OF CHAR);
VAR
  r  : Registers;
  fn : PathStr;
  D  : ARRAY[0..2] OF CHAR;
BEGIN
  D[1]:=':'; D[2]:=FileName.root;
  WITH r DO
    AH := 19H;
    Lib.Dos(r);
    D[0] := CHAR(AL+65);
    DS := Seg(fn);
    SI := Ofs(fn);
    DL := AL+1;
    AH := 47H;
    Lib.Dos(r);
    ErrorCheck(r, 'GetCurrent');
  END;
  String.Concat(Name,D,fn);
END GetCurrent;

PROCEDURE GetParent(VAR dir: ARRAY OF CHAR);
VAR
  full: ARRAY[0..127] OF CHAR;
  name: ARRAY[0..14] OF CHAR;
BEGIN
  GetCurrent(full);
  FileName.Split(full,dir,name);
  IF dir[String.Length(dir)-1]=':' THEN
    String.Append(dir,FileName.delim);
    SetError(pathNotFoundError, 'GetParent');
  END;
END GetParent;

END Dir.

