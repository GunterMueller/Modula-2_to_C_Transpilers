(* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'FileName' module. It contains operations for
    deleting and renaming the files. Operations are characteristic for
    80x86/MS-DOS platform and means for doing them in
    Top Speed Modula-2 ver. 1.x.

    by Zoran Budimac, December, 1996
    ------------------------------------------------------------------ *)
(*$S-,R-,I-*)
IMPLEMENTATION MODULE TS1FileNames;
FROM SYSTEM IMPORT Registers, CarryFlag, Seg, Ofs;
IMPORT Lib;
IMPORT String;
FROM FileName IMPORT SetError,
              fileNotFoundError, pathNotFoundError, priviledgeError,
              unknownError;
(*$V-*)
TYPE
  PathStr  = ARRAY[0..64] OF CHAR;
  PathTail = ARRAY[0..12] OF CHAR;

PROCEDURE ErrorCheck ( VAR r : Registers; msg: ARRAY OF CHAR);
VAR
  S:CARDINAL;
BEGIN
  WITH r DO
     IF (BITSET{CarryFlag} * Flags) # BITSET{} THEN
       S := AX;
       CASE S OF
          2 : SetError(fileNotFoundError, msg);
       |  3 : SetError(pathNotFoundError, msg);
       |  5 : SetError(priviledgeError, msg);
       ELSE
         SetError(unknownError, msg);
       END;
     END;
  END;
END ErrorCheck;

PROCEDURE GetName(name: ARRAY OF CHAR; VAR fn: PathStr);
BEGIN
  String.Copy(fn,name);
  fn[SIZE(fn)-1] := CHR(0);
END GetName;

PROCEDURE Delete(Name:ARRAY OF CHAR);
VAR
  r  : Registers;
  fn : PathStr;
BEGIN
  GetName(Name,fn);
  WITH r DO
    DS := Seg(fn);
    DX := Ofs(fn);
    AH := 41H;
    Lib.Dos(r);
    IF ((BITSET{CarryFlag} * Flags) # BITSET{}) AND (AX = 2) THEN
      Flags := BITSET{};
    END;
    ErrorCheck(r, 'Delete');
  END;
END Delete;

PROCEDURE Rename(Name,newname: ARRAY OF CHAR);
VAR
  r : Registers;
  fn,fn2: PathStr;
BEGIN
  GetName(Name,fn);
  GetName(newname,fn2);
  WITH r DO
    DS := Seg(fn);
    DX := Ofs(fn);
    ES := Seg(fn2);
    DI := Ofs(fn2);
    AH := 56H;
    Lib.Dos(r);
    ErrorCheck(r, 'Rename');
  END;
END Rename;

END TS1FileNames.