(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       File (1.1)                                *
 * Version:                 1.0                                       *
 * Date:                    August 18, 1996                           *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler Version 1.xx   *
 * Portability:             This module is pretty non-portable. Every *
 *                          port should provide its own module File.  *
 * To do:                   Try (if at all possible) to place non-    *
 *                          -poratble code in separate module (like   *
 *                          in module Date has been done              *
 *                                                                    *
 * Author:                  Daniel Sendula                            *
 *                          (for 'Black Mailbox' Group)               *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: isendula@unsim.ns.ac.yu       *
 * ------------------------------------------------------------------ *)

(*$S-,R-,I-*)
IMPLEMENTATION MODULE File;

FROM SYSTEM IMPORT Seg,Ofs,Registers,CarryFlag;
FROM Storage IMPORT ALLOCATE, DEALLOCATE;

IMPORT Lib;
IMPORT String;      (* BMB string operations *)
IMPORT Wr;

TYPE
        T = POINTER TO TRec;
        TRec = RECORD
              F: CARDINAL;           (* File type *)
              LastChar: CHAR;
              OneMoreChar: BOOLEAN;
              Interactive: BOOLEAN;
              unread: BOOLEAN;
            END;


CONST
  MaxOpenFiles = 15;
  DiskIsFull = 0F0H ; (* Error if Write fails with disk full *)
  MaxHandle  = MaxOpenFiles+3;

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
  Err      : CARDINAL; (* MINE *)
  IOResult : CARDINAL; (* MINE *)
  errorCode: CARDINAL;
  EndOfLine: ARRAY[0..1] OF CHAR;

  BufInf : ARRAY[0..MaxHandle] OF FileInf;

  StdInT, StdOutT, StdErrT, StdPrnT: T;

(* ----------------------------------------------------------------- *)

PROCEDURE ClearError;
BEGIN
  Err := noError;
END ClearError;

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
  Wr.Str('*** File.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError      : Wr.Str('Unknown error');
  | diskFullError     : Wr.Str('Disk is full');
  | noRoomError       : Wr.Str('Buffer exceeded');
  | fileNotFoundError : Wr.Str('File not found');
  | pathNotFoundError : Wr.Str('Path not found');
  | readError         : Wr.Str('Read error');
  | writeError        : Wr.Str('Write error');
  | priviledgeError   : Wr.Str('Priviledge error');
  | handleError       : Wr.Str('Wrong handle');
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
          2 : SetError(fileNotFoundError, msg);
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

PROCEDURE StdIn(): T;
BEGIN
  RETURN StdInT
END StdIn;

PROCEDURE StdOut(): T;
BEGIN
  RETURN StdOutT
END StdOut;

PROCEDURE StdErr(): T;
BEGIN
  RETURN StdErrT
END StdErr;

PROCEDURE StdPrn(): T;
BEGIN
  RETURN StdPrnT
END StdPrn;

PROCEDURE GetName(name: ARRAY OF CHAR; VAR fn: PathStr);
BEGIN
  String.Copy(fn,name);
  fn[SIZE(fn)-1] := CHR(0);
END GetName;

PROCEDURE OpenMode(Name: ARRAY OF CHAR; mode: WORD) : T;
VAR
  r  : Registers;
  fn : PathStr;
  ret: T;
BEGIN
  NEW(ret);
  ret^.LastChar := 0C;
  ret^.OneMoreChar := FALSE;
  ret^.Interactive := FALSE;
  ret^.unread := FALSE;
  GetName(Name,fn);
  WITH r DO
    DS := Seg(fn);
    DX := Ofs(fn);
    CX := 0;
    AX := mode;    (* open modus *)
    Lib.Dos(r);
    IF ((BITSET{CarryFlag} * Flags) # BITSET{}) AND (AX = 5) THEN
      (* access denied *)
      AX := 3D00H; (* open for read *)
      Lib.Dos(r);
    END;
    ErrorCheck(r, 'OpenMode');
      (* Need to be optimized - string passed to procedure each time. *)
    IF Error() = 0 THEN
      IF AX <= MaxHandle THEN BufInf[AX] := NIL; END;
      ret^.F := AX;
      RETURN ret
    ELSE
      ret^.F := MAX(CARDINAL);
      RETURN ret
    END;
  END;
END OpenMode;

PROCEDURE Open(Name: ARRAY OF CHAR) : T;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
   RETURN OpenMode(Name, 3D02H);
END Open;

PROCEDURE OpenRead(Name: ARRAY OF CHAR): T;
   (* by Zoran Budimac, February, 1995 *)
BEGIN
   RETURN OpenMode(Name, 3D00H);
END OpenRead;

PROCEDURE Append(Name: ARRAY OF CHAR) : T;
VAR F : T;
BEGIN
  F := Open( Name );
  Seek( F,Size( F ) );
  RETURN F;
END Append;

PROCEDURE Create(Name: ARRAY OF CHAR) : T;
VAR
  ret: T;
  r  : Registers;
  fn : PathStr;
BEGIN
  NEW(ret);
  ret^.LastChar := 0C;
  ret^.OneMoreChar := FALSE;
  ret^.Interactive := FALSE;
  ret^.unread := FALSE;
  GetName(Name,fn);
  WITH r DO
    DS := Seg(fn);
    DX := Ofs(fn);
    CX := 0;
    AH := 3CH;
    Lib.Dos(r); (* Create, open for read and write *)
    ErrorCheck(r, 'Create');
    IF Error() = 0 THEN
      IF AX <= MaxHandle THEN BufInf[AX] := NIL; END;
      ret^.F := AX;
      RETURN ret
    ELSE
      ret^.F := MAX(CARDINAL);
      RETURN ret
    END;
  END;
END Create;

PROCEDURE Close(f: T);
VAR
  r : Registers;
  x : FileInf;
BEGIN
  Flush( f );
  IF f^.F <= MaxHandle THEN
    BufInf[f^.F] := NIL;
  END;
  WITH r DO
    BX := f^.F;
    AH := 3EH; (* close file *)
    Lib.Dos(r);
    ErrorCheck(r, 'Close');
  END;
  IF NOT f^.Interactive THEN
    DISPOSE(f);
  END;
END Close;

PROCEDURE IsInteractive(f: T): BOOLEAN;
BEGIN
  IF f^.F < 4 THEN
    RETURN TRUE
  ELSE
    RETURN FALSE
  END;
END IsInteractive;

PROCEDURE Write(f: T; Buf: ARRAY OF BYTE; Count: LONGCARD);
VAR r : Registers;
    ok: BOOLEAN;
    seg,ofs: CARDINAL;
BEGIN
  ClearError;
  IF Count = 0 THEN RETURN END;
  ok:=TRUE;
  seg:=Seg(Buf);
  ofs:=Ofs(Buf);
  REPEAT
    WITH r DO
      DS := seg;
      DX := ofs;
      AH := 40H;  (* file write *)
      BX := f^.F;
      (* IF Count>32768 THEN
        CX := 32768;
        Count := Count - 32768;
        seg:=seg+(32768 DIV 16);
      ELSE *)
        CX := CARDINAL(Count);
        ok := FALSE;
      (* END; *)
      Lib.Dos(r);
      IF (AX # CX) AND ((BITSET{CarryFlag} * Flags) = BITSET{}) THEN
        AX := DiskIsFull;
        Flags := BITSET{CarryFlag};
        ok:=FALSE;
      END;
    END;
  UNTIL NOT ok;
  ErrorCheck(r, 'Write');
END Write;

PROCEDURE WrBin(f: T; VAR bin: ARRAY OF BYTE; size: LONGCARD);
BEGIN
  Write(f,bin,size);
END WrBin;

PROCEDURE WrStr(f: T; s: ARRAY OF CHAR);
VAR Count : LONGCARD;
BEGIN
  Count := LONGCARD(String.Length( s ));
  Write( f,s, Count );
END WrStr;

PROCEDURE WrLn(f: T);
BEGIN
  Write( f,EndOfLine,2 );
END WrLn;

PROCEDURE WrChar(f: T; ch: CHAR);
BEGIN
  Write( f,ch,1 );
END WrChar;

PROCEDURE Read(f: T; VAR Buf: ARRAY OF BYTE; Count: LONGCARD) : LONGCARD;
VAR r : Registers;
    ok: BOOLEAN;
    seg,ofs: CARDINAL;
    Actual: LONGCARD;
BEGIN
  ClearError;
  IF Count = 0 THEN RETURN 0 END;
  ok := TRUE;
  IF f^.OneMoreChar THEN
    f^.OneMoreChar := FALSE;
    Buf[0]:=BYTE(f^.LastChar);
    DEC(Count);
    IF Count = 0 THEN RETURN 1 END;
    Actual:=1;
    seg:=Seg(Buf);
    ofs:=Ofs(Buf);
    IF ofs=65535 THEN
      ofs := 65535-15;
      INC(seg)
    END;
  ELSE
    seg:=Seg(Buf);
    ofs:=Ofs(Buf);
    Actual:=0;
  END;
  REPEAT
    WITH r DO
      DS := seg;
      DX := ofs;
      AH := 3FH; (* file read *)
      BX := f^.F;
      (* IF Count>32768 THEN
        CX := 32768;
        Count:=Count-32768;
        seg:=seg+(32768 DIV 16);
      ELSE *)
        CX := CARDINAL(Count);
        ok := FALSE;
      (* END; *)
      Lib.Dos(r);
      ErrorCheck(r, 'Read');
      IF Error() <> noError THEN
        ok:=FALSE;
      ELSE
        Actual:=Actual+LONGCARD(AX);
      END;
    END;
  UNTIL NOT ok;
  IF Error() = noError THEN
    RETURN Actual;
  ELSE
    RETURN Actual;
  END;
  f^.unread := FALSE;
END Read;

PROCEDURE RdBin(f: T; VAR bin: ARRAY OF BYTE; size: LONGCARD): LONGCARD;
BEGIN
  RETURN Read(f,bin,size);
END RdBin;

PROCEDURE RdChar(f: T ) : CHAR;
VAR c : CHAR;
    s : ARRAY [0..21] OF CHAR;
BEGIN
  IF f^.OneMoreChar THEN
    f^.OneMoreChar := FALSE;
    f^.unread := TRUE;
    RETURN f^.LastChar
  ELSE
    IF Read(f,c,1) = 0 THEN
      f^.unread := FALSE;
      RETURN 0C;
    ELSE
      f^.LastChar := c;
      f^.unread := TRUE;
      RETURN c;
    END;
  END;
END RdChar;

PROCEDURE UnRdChar(VAR f: T);
BEGIN
 (*  Seek(f, Pos(f)-1); *)
   IF f^.unread THEN
     f^.OneMoreChar := TRUE;
     f^.unread := FALSE;
   ELSE
     SetError(unknownError, "UnRdChar");
   END;
END UnRdChar;

PROCEDURE RdLine(f: T; VAR line: ARRAY OF CHAR);
VAR
  c:CHAR;
  p:CARDINAL;
BEGIN
  ClearError;
  p:=0;
  LOOP
    c:=RdChar(f);
    IF (c=15C) OR (c=12C) OR (c=14C) THEN
        (* Modification by Z. Budimac, March, 1995 *)
      IF c=15C THEN
         c:=RdChar(f)
      END;
      IF p<=HIGH(line) THEN
        line[p]:=0C;
      END;
      RETURN
    ELSE
      IF Error()>0 THEN
        RETURN
      ELSE
        IF p>HIGH(line) THEN
          SetError(noRoomError , 'RdLine');
          RETURN
        ELSE
          line[p]:=c;
          INC(p);
        END;
      END;
    END;
  END; (* LOOP *)
  f^.unread := FALSE;
END RdLine;

PROCEDURE RdLn(f: File.T);
   (* by Zoran Budimac, february 1995 *)
VAR s: ARRAY [0..999] OF CHAR;
BEGIN
  RdLine(f, s);
  f^.unread := FALSE;
END RdLn;

PROCEDURE Size(f: T) : LONGCARD;
VAR
  r     : Registers;
  conv1,
  conv2 : RECORD
            CASE : BOOLEAN OF
             | TRUE  : lo,hi: CARDINAL;
             | FALSE : long : LONGCARD;
            END;
          END;

BEGIN
  Flush( f );
  WITH r DO
    CX := 0;
    DX := 0;
    AX := 4201H;   (* seek relative *)
    BX := f^.F;
    Lib.Dos(r);
    conv1.hi := DX;
    conv1.lo := AX;
    CX := 0;
    DX := 0;
    AX := 4202H;   (* seek relative to end *)
    Lib.Dos(r);
    conv2.hi := DX;
    conv2.lo := AX;
    CX := conv1.hi;
    DX := conv1.lo;
    AX := 4200H;   (* seek absolute *)
    Lib.Dos(r);
    ErrorCheck(r, 'Size');
  END;
  RETURN conv2.long;
END Size;

PROCEDURE Pos(f: T) : LONGCARD;
VAR
  r    : Registers;
  x    : FileInf;
  conv : RECORD
           CASE : BOOLEAN OF
            | TRUE  : lo,hi: CARDINAL;
            | FALSE : long : LONGCARD;
           END;
         END;
BEGIN
  WITH r DO
    CX := 0;
    DX := 0;
    AX := 4201H; (* seek relative *)
    BX := f^.F;
    Lib.Dos(r);
    ErrorCheck(r, 'Pos');
    conv.lo := AX;
    conv.hi := DX;
    IF (f^.F <= MaxHandle) THEN
      x := BufInf[f^.F];
      IF (x # NIL) THEN
        WITH x^ DO
          IF RWPos > EOB THEN
            INC(conv.long,VAL(LONGCARD,RWPos));
          ELSIF RWPos < EOB THEN
            DEC(conv.long,VAL(LONGCARD,EOB-RWPos));
          END;
        END;
      END;
    END;
    RETURN conv.long;
  END;
END Pos;

PROCEDURE EOF(f: T):BOOLEAN;
BEGIN

  IF Pos(f) >= Size(f) THEN
    RETURN TRUE;
  ELSE
    RETURN FALSE;
  END;
END EOF;

PROCEDURE Seek( f : T; pos:LONGCARD );
VAR
  EOBpos : LONGCARD;
  r    : Registers;
  conv : RECORD
           CASE : BOOLEAN OF
            | TRUE  : lo,hi: CARDINAL;
            | FALSE : long : LONGCARD;
           END;
         END;

BEGIN
  IF (f^.F<=MaxHandle) AND (BufInf[f^.F]<>NIL) THEN
    WITH BufInf[f^.F]^ DO
      IF (EOB>0) AND (RWPos<=EOB) THEN
        (* there are read bytes in the buffer *)
        (* find out where we are in the file *)
        WITH r DO
          CX := 0; DX := 0;
          AX := 4201H; (* seek relative *)
          BX := f^.F;
          Lib.Dos(r); ErrorCheck(r, 'Seek');
          conv.lo := AX; conv.hi := DX; EOBpos := conv.long;
        END;
        IF (pos>=EOBpos-LONGCARD(EOB)) AND (pos<EOBpos) THEN
          (* we can seek within the buffer *)
          RWPos := EOB - CARDINAL(EOBpos-pos);
          RETURN;
        END;
      ELSE
        IF RWPos > EOB THEN (* last was write, bring DOS up to date *)
          Write( f,Buffer, LONGCARD(RWPos));
        END;
      END;
      (* the buffer contents are finished with now *)
      RWPos := 0;
      EOB := 0;
    END;
  END;
  (* we haven't done an in-buffer seek - so get DOS to do it for us *)
  conv.long := pos;
  WITH r DO
    CX := conv.hi;
    DX := conv.lo;
    AX := 4200H; (* seek absolute *)
    BX := f^.F;
    Lib.Dos(r);
    ErrorCheck(r, 'Seek');
  END;
END Seek;

PROCEDURE Flush(f: T);
VAR x : FileInf;
  r : Registers;
  conv : RECORD
           CASE : BOOLEAN OF
            | TRUE  : lo,hi: CARDINAL;
            | FALSE : long : LONGINT;
           END;
         END;
BEGIN
  IF f^.OneMoreChar THEN
    f^.OneMoreChar := FALSE
  END;
  IF (f^.F <= MaxHandle) THEN
    x := BufInf[f^.F];
    IF (x # NIL) THEN
      WITH x^ DO
        IF RWPos > EOB THEN
          Write( f,Buffer, LONGCARD(RWPos));
        ELSIF RWPos < EOB THEN (* last was read, move DOS pointer back *)
          conv.long := LONGINT(RWPos) - LONGINT(EOB);
          WITH r DO
            CX := conv.hi;
            DX := conv.lo;
            AX := 4201H; (* seek relative *)
            BX := f^.F;
            Lib.Dos(r);
            ErrorCheck(r, 'Flush');
          END;
        END;
        RWPos := 0;
        EOB   := 0;
      END;
    END;
  END;
END Flush;

PROCEDURE InitBufInf;
VAR i : CARDINAL;
BEGIN
  FOR i := 0 TO MaxHandle DO BufInf[i] := NIL; END;
END InitBufInf;

BEGIN
  errorCode := 0;
  errorHandler := PrintError;

  InitBufInf;
  EndOfLine[0]:=CHR(13); EndOfLine[1]:=CHR(10);
  NEW(StdInT);
  NEW(StdOutT);
  NEW(StdErrT);
  NEW(StdPrnT);
  StdInT^.F:=0;
  StdInT^.OneMoreChar:=FALSE;
  StdInT^.LastChar:=0C;
  StdInT^.Interactive:=TRUE;
  StdOutT^:=StdInT^;
  StdErrT^:=StdInT^;
  StdPrnT^:=StdInT^;
  StdOutT^.F:=1;
  StdErrT^.F:=2;
  StdPrnT^.F:=4;
  Flush(StdInT);
  Seek(StdIn(),Pos(StdIn()));
  Flush(StdOutT);
  Flush(StdErrT);
  Flush(StdPrnT);
END File.