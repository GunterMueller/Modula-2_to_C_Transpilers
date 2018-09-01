(* ------------------------------------------------------------------- *
 *              BMB Modula-2 Cross-Platform Library                    *
 *                                                                     *
 * Implementation of:       DirEntry                                   *
 *                          (see DirEntry.doc for full specification!) *
 * Version:                 1.10                                       *
 * Date:                    October, 1996                              *
 * Conformance Test Passed: -                                          *
 * Platform:                80x86 / MS-DOS                             *
 * Requires:                TopSpeed Modula-2 ver. 1.x                 *
 * Acknowledgement:         Implementation based on TS FIO module      *
 *                                                                     *
 * Author:                Zoran Budimac                                *
 *                        (for 'Black Mailbox' Group)                  *
 *                            University of Novi Sad, Yugoslavia       *
 *                            e-mail: zjb@unsim.ns.ac.yu               *
 * ------------------------------------------------------------------- *)

IMPLEMENTATION MODULE DirEntry;
FROM SYSTEM IMPORT Registers, Seg, Ofs, CarryFlag;
IMPORT Lib;

IMPORT String, Wr;

TYPE
  PathTail = ARRAY[0..12] OF CHAR;
  PathStr  = ARRAY[0..64] OF CHAR;
  FileAttr = SET OF (readonly,hidden,system,volume,directory,archive);
  DOSEntry = RECORD
               rsvd : ARRAY[0..20] OF SHORTCARD;
               attr : FileAttr;
               time : CARDINAL;
               date : CARDINAL;
               size : LONGCARD;
               Name : PathTail;
             END;

VAR
  errorCode : CARDINAL;

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
   (* by Daniel Sendula, February, 1995 *)
BEGIN
  Wr.Str('*** DirEntry.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError     : Wr.Str('Unknown error');
  | noRoomError      : Wr.Str('Buffer exceeded');
  | pathNotFoundError: Wr.Str('Path not found');
  | priviledgeError  : Wr.Str('Priviledge error');
  END;
  HALT;
END PrintError;

PROCEDURE ErrorCheck ( VAR r : Registers; msg: ARRAY OF CHAR);
   (* by Daniel Sendula, February 1996 *)
VAR
  S:CARDINAL;
BEGIN
  WITH r DO
     IF (BITSET{CarryFlag} * Flags) # BITSET{} THEN
       S := AX;
       CASE S OF
       |  3 : SetError(pathNotFoundError, msg);
       |  5 : SetError(priviledgeError, msg);
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

PROCEDURE ReadFirstEntry ( DirName : ARRAY OF CHAR;
                           Attr    : FileAttr;
                           VAR D   : DOSEntry) : BOOLEAN;
VAR
  r  : Registers;
  fn : PathStr;
BEGIN
  GetName(DirName,fn);
  WITH r DO
    AH := 1AH;
    DS := Seg(D);
    DX := Ofs(D);
    Lib.Dos(r);    (* set DTA *)
    AH := 4EH;
    DS := Seg(fn);
    DX := Ofs(fn);
    CL := SHORTCARD(Attr);
    Lib.Dos(r);
    IF ((BITSET{CarryFlag} * Flags) # BITSET{}) AND (AX = 18) THEN
      RETURN FALSE;
    END;
    ErrorCheck(r, 'ReadFirstEntry');
  END;
  RETURN TRUE;
END ReadFirstEntry;

PROCEDURE ReadNextEntry(VAR D: DOSEntry) : BOOLEAN;
VAR
  r : Registers;
BEGIN
  WITH r DO
    AH := 1AH;
    DS := Seg(D);
    DX := Ofs(D);
    Lib.Dos(r);    (* set DTA *)
    AH := 4FH;
    Lib.Dos(r);
    IF ((BITSET{CarryFlag} * Flags) # BITSET{}) AND (AX = 18) THEN
      RETURN FALSE;
    END;
    ErrorCheck(r, 'ReadNextEntry');
  END;
  RETURN TRUE;
END ReadNextEntry;


PROCEDURE CrTime(VAR t: Time.T; e: T);
BEGIN
  t := e.timeC
END CrTime;

PROCEDURE ModTime(VAR t: Time.T; e: T);
BEGIN
  t := e.timeM
END ModTime;

PROCEDURE CrDate(VAR d: Date.T; e: T);
BEGIN
  d := e.dateC
END CrDate;

PROCEDURE ModDate(VAR d: Date.T; e: T);
BEGIN
  d := e.dateM
END ModDate;

PROCEDURE Size(e: T): LONGCARD;
BEGIN
  RETURN e.size
END Size;

PROCEDURE Name(VAR name: ARRAY OF CHAR; e: T);
BEGIN
  String.Copy(name, e.name);
END Name;

PROCEDURE ConvertTime(VAR t: Time.T; n: CARDINAL);
BEGIN
  Time.SetHour(t, n DIV 2048);
  n := n MOD 2048;
  Time.SetMin(t, n DIV 32);
  n := n MOD 32;
  Time.SetSec(t, n);
  Time.SetHund(t, 0);
END ConvertTime;

PROCEDURE ConvertDate(VAR d: Date.T; n: CARDINAL);
BEGIN
  Date.SetYear(d, (n DIV 512)+1980);
  n := n MOD 512;
  Date.SetMonth(d, n DIV 32);
  n := n MOD 32;
  Date.SetDay(d, n);
END ConvertDate;

VAR eDOS: DOSEntry;   (* DOS Directory entry *)
    currentFileName, previousFileName: ARRAY [0..125] OF CHAR;
                      (* For directory entries *)

PROCEDURE SetPath(filename: ARRAY OF CHAR);
BEGIN
   String.Copy(currentFileName, filename)
END SetPath;

PROCEDURE Found(VAR e: T): BOOLEAN;
VAR  val: BOOLEAN;
BEGIN
  IF String.Equal(previousFileName, currentFileName) THEN
     val := ReadNextEntry(eDOS);
  ELSE
     String.Copy(previousFileName, currentFileName);
     val := ReadFirstEntry(currentFileName,
                           FileAttr{readonly,hidden,system,volume,directory,archive},
                           eDOS);
  END;
  e.size := eDOS.size;
  ConvertTime(e.timeC, eDOS.time);
  ConvertDate(e.dateC, eDOS.date);
  e.timeM := e.timeC;
  e.dateM := e.dateC;
  String.Copy(e.name, eDOS.Name);
  RETURN val;
END Found;

BEGIN
  errorCode := 0;
  String.Copy(previousFileName, '');
END DirEntry.