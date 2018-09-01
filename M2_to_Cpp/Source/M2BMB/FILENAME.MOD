(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       FileName (1.1)                            *
 * Version:                 1.0                                       *
 * Date:                    October 20, 1996                          *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler Version 1.xx   *
 * Remarks:                 Non-portable operations are contained in  *
 *                          separate module (TS1FileNames). To port   *
 *                          this module, provide XXXFileNames module  *
 *                          for XXX platform                          *
 *                                                                    *
 * Author:                  Daniel Sendula                            *
 *                          (for 'Black Mailbox' Group)               *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: isendula@unsim.ns.ac.yu       *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE FileName;
IMPORT String, Wr, DirEntry;

VAR
  errorCode: CARDINAL;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** FileName.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError     : Wr.Str('Unknown error');
  | noRoomError      : Wr.Str('Buffer exceeded');
  | notFileError     : Wr.Str('It is not a file');
  | fileNotFoundError: Wr.Str('File not found');
  | pathNotFoundError: Wr.Str('Path not found');
  | priviledgeError : Wr.Str('Priviledge error');
  END;
  HALT;
END PrintError;

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

(* ----------------------------------------------------------------- *)

PROCEDURE Exists(Name: ARRAY OF CHAR) : BOOLEAN;
VAR
  e: DirEntry.T;
BEGIN
  DirEntry.SetPath(Name);
  RETURN DirEntry.Found(e);
END Exists;

PROCEDURE Split(fullName: ARRAY OF CHAR;
                VAR dir, name: ARRAY OF CHAR);
   (* Splits the full file name 'fullName' into directory
      name (path) 'dir' and proper filename 'name'. Device
      name is the part of directory name 'dir' *)
VAR
  p,sp,len,i:CARDINAL;
BEGIN
  len := String.Length(fullName);
  p := len+1;
  sp := 0;
  LOOP
    IF p = 0 THEN EXIT END;
    DEC(p);
    IF (fullName[p]=root) THEN
      IF p > 0 THEN
        sp := p;
      END;
      EXIT;
    END;
  END;
  IF sp > 0 THEN
    DEC(sp);
    IF HIGH(dir) >= sp THEN
      FOR i := 0 TO sp DO
        dir[i] := fullName[i];
      END;
      IF HIGH(dir) > sp THEN
        dir[sp+1] := 0C;
      END;
    ELSE
      SetError(noRoomError, 'Split');
    END;
    INC(sp);
  ELSE
    dir[0] := 0C;
  END;
  IF sp > 0 THEN INC(sp) END;
  IF sp < len THEN
    IF HIGH(name) >= len-sp THEN
      FOR i := sp TO len-1 DO
        name[i-sp] := fullName[i];
      END;
      IF HIGH(name) > len-sp THEN
        name[len-sp] := 0C
      END;
    END;
  ELSE
    name[0] := 0C;
  END;
END Split;

PROCEDURE Concat(VAR fullName:ARRAY OF CHAR;
                 dir, name: ARRAY OF CHAR);
   (* Concats the names of directory name (path) 'dir' and
      proper filename 'name' into a full file name 'fullName'.
      Device name is the part of directory name 'dir' *)
BEGIN
  String.Concat(fullName, dir, delim);
  String.Append(fullName, name);
END Concat;

END FileName.

