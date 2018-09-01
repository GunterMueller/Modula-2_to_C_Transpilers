(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Scr (1.1)                                 *
 * Version:                 1.0                                       *
 * Date:                    December 27, 1996                         *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * Portability:             Non-portable operations                   *
 *                                                                    *
 * Author:                      Bojan Skaljak                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                                                                    *
 * Modifications:               Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Scr;
FROM SYSTEM IMPORT Registers, Ofs, Seg;
IMPORT Lib;
FROM Lib IMPORT Intr, WordFill;
IMPORT ShortMem, Wr;


VAR
   errorCode : CARDINAL;
   maxX, maxY: CARDINAL;

   cursorOn : BOOLEAN; (* Is cursor currently on ? *)

(*=========================================================================*)
(* Procedures for error handling.                                          *)
(*=========================================================================*)

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

(*=========================================================================*)
(* Following two procedures turn on and off cursor by using BIOS VIDEO 10H *)
(* function , subfunction 01H.BIOS DATA AREA location 0000:0485H contains  *)
(* height of currently used font which is used to set shape of the cursor. *)
(* Bits from 0 to 4 in CH(CL) contains top(bottom) line of cursor, while   *)
(* bits 5 & 6 in CH are responsible for turning on(combination 00) and     *)
(* off(combination 01) cursor.Other bits are not used.                     *)
(*=========================================================================*)

PROCEDURE SetCursorOn;
VAR
  r:Registers;
BEGIN
  r.AH:=01H;
  r.CH:=00;
  r.CL:=BYTE([0000:0485H]^);
  Intr(r,10H);
  cursorOn := TRUE;
END SetCursorOn;

PROCEDURE SetCursorOff;
VAR
  r:Registers;
BEGIN
  r.AH:=01H;
  r.CH:=BYTE([0000:0485H]^)+BYTE(20H); (* Sets bit No_5 *)
  r.CL:=BYTE([0000:0485H]^);
  Intr(r,10H);
  cursorOn := FALSE;
END SetCursorOff;

PROCEDURE CursorOn(): BOOLEAN;
BEGIN
  RETURN cursorOn;
END CursorOn;

(*==========================================================================*)
(* Gets maximum coordinates of the display screen.                          *)
(*==========================================================================*)

PROCEDURE GetMaxCoords(VAR x, y: CARDINAL);
VAR
  r:Registers;
BEGIN
  r.AH:=11H;
  r.AL:=30H;
  r.BH:=0;
  Intr(r,10H); (* requesting information from BIOS,about default font. *)
  y:=CARDINAL(r.DL);
  r.AH:=0FH;
  Intr(r,10H);
  x:=CARDINAL(r.AH);
END GetMaxCoords;

(*============================================================================*)
(* Next three procedures manipulate with cursor using standard BIOS service   *)
(* interrupt 10h,subfunctions 02h and BIOS DATA area.BIOS subfunction 10h,02  *)
(* requires current screen page in register BH, row in DH and column in DL.   *)
(* BIOS DATA area, addresses 0450H-045FH contains current cursor x,y position *)
(* for all available screen pages, while current page can be read from address*)
(* [0000:0462H].                                                              *)
(*============================================================================*)

PROCEDURE CursorX(): CARDINAL;
VAR
  currentPage:CARDINAL;
BEGIN
  currentPage:=CARDINAL(SHORTCARD([0000:0462]^));
  RETURN CARDINAL(BYTE([0000:0450H+currentPage*2]^));
END CursorX;

PROCEDURE CursorY(): CARDINAL;
VAR
  currentPage:CARDINAL;
BEGIN
  currentPage:=CARDINAL(SHORTCARD([0000:0462]^));
  RETURN CARDINAL(BYTE([0000:0450H+currentPage*2+1]^));
END CursorY;

PROCEDURE SetCursor(x, y: CARDINAL);
VAR
  r:Registers;
BEGIN
  r.AH:=02;
  r.BH:=BYTE([0000:0462H]^); (* Current Page *)
  r.DH:=SHORTCARD(y);
  r.DL:=SHORTCARD(x);
  Intr(r,10H);
END SetCursor;

PROCEDURE Fill(ch: CHAR; foreground, background: CARDINAL);
   (* Fills entire screen with 'ch' writing it in 'foreground' color
      on 'background' color *)
VAR
  attr: WORD;
BEGIN
  attr := background<<12 + foreground<<8;
  WordFill([0B800H:0000H], maxX*maxY, attr+WORD(ch));
END Fill;

PROCEDURE Save(VAR buffer: ARRAY OF BYTE);
   (* by Zoran Budimac, January, 1997 *)
BEGIN
  ShortMem.Copy(ADR(buffer), [0B800H:0000H], HIGH(buffer)+1);
END Save;

PROCEDURE Restore(VAR buffer: ARRAY OF BYTE);
   (* by Zoran Budimac, January, 1997 *)
BEGIN
  ShortMem.Copy([0B800H:0000H], ADR(buffer), HIGH(buffer)+1);
END Restore;

PROCEDURE SaveBlock(x, y, width, height: CARDINAL; VAR buffer: ARRAY OF BYTE);
   (* by Zoran Budimac, January, 1997 *)
VAR
  m: CARDINAL;
  offsFrom, offsTo, i: CARDINAL;
BEGIN
  m := HIGH(buffer)+1;
  ShortMem.Copy(ADR(buffer), [0B800H:0000H], HIGH(buffer)+1);
  i := y;
  offsTo := Ofs(ADR(buffer));
  WHILE i < y+height DO
      offsFrom := (i*maxX)<<1 + x<<1;
      ShortMem.Copy([offsTo:Seg(ADR(buffer))], [0B800H:offsFrom], width<<1);
      offsTo := offsTo + width;
      INC(i)
  END;

END SaveBlock;

PROCEDURE RestoreBlock(x, y, width, height: CARDINAL; VAR buffer: ARRAY OF BYTE);
   (* by Zoran Budimac, January, 1997 *)
VAR
  m: CARDINAL;
  offsFrom, offsTo, i: CARDINAL;
BEGIN
  m := HIGH(buffer)+1;
  ShortMem.Copy(ADR(buffer), [0B800H:0000H], HIGH(buffer)+1);
  i := y;
  offsTo := Ofs(ADR(buffer));
  WHILE i < y+height DO
      offsFrom := (i*maxX)<<1 + x<<1;
      ShortMem.Copy([0B800H:offsFrom], [offsTo:Seg(ADR(buffer))], width<<1);
      offsTo := offsTo + width;
      INC(i)
  END;

END RestoreBlock;

PROCEDURE FillBlock(ch: CHAR; x, y, width, heigth, foreground, background: CARDINAL);
   (* Clears block on the screen between coordinates (x,y) and
      (x+width, y+heigth) setting 'foreground' color on
      'background' *)
VAR
  offset, i:CARDINAL;
  attr: WORD;
BEGIN
  attr := background<<12 + foreground<<8;
  FOR i := y TO y+heigth-1 DO
      offset := x<<1 + (i*maxX)<<1;
      WordFill([0B800H:offset], width, attr+WORD(ch));
  END;
END FillBlock;

PROCEDURE CopyBlock(x, y, width, height, newX, newY: CARDINAL);
   (* by Zoran Budimac, January, 1997 *)
VAR
  offsFrom, offsTo, i: CARDINAL;
BEGIN
  IF y < newY THEN
    FOR i := height-1 TO 0 BY -1 DO
      offsTo   := ((newY+i)*maxX)<<1 + newX<<1;
      offsFrom := ((y+i)*maxX)<<1 + x<<1;
      ShortMem.Copy([0B800H:offsTo], [0B800H:offsFrom], width<<1);
    END;
  ELSE
    FOR i := 0 TO height-1 DO
      offsTo   := ((newY+i)*maxX)<<1 + newX<<1;
      offsFrom := ((y+i)*maxX)<<1 + x<<1;
      ShortMem.Copy([0B800H:offsTo], [0B800H:offsFrom], width<<1);
    END;
  END;
END CopyBlock;

PROCEDURE WrCharAt(ch: CHAR; x, y, foreground, background: CARDINAL);
   (* by Zoran Budimac, January, 1997 *)
   (* Writes 'ch' at coordinates ('x','y') using 'foreground' color
      on 'background' *)
VAR attr: WORD;
    offset: CARDINAL;
BEGIN
   attr := background<<12 + foreground<<8;
   offset := (y*maxX)<<1 + x<<1;
   WordFill([0B800H:offset], 1, attr+WORD(ch));
END WrCharAt;

PROCEDURE WrStrAt(str: ARRAY OF CHAR; x, y, foreground, background: CARDINAL);
   (* by Daniel Sendula, May, 1997 *)
   (* Writes 'str' at coordinates ('x','y') using 'foreground' color
      on 'background'. If it falls out of screen it will be clipped. *)
VAR attr: WORD;
    offset: CARDINAL;
    maxy, maxx, i: CARDINAL;
    ch: CHAR;
BEGIN
   attr := background<<12 + foreground<<8;
   offset := (y*maxX)<<1 + x<<1;
   GetMaxCoords(maxx, maxy);
   i := 0;
   LOOP
     IF x > maxx THEN RETURN END;
     IF str[i]=0C THEN RETURN END;
     WordFill([0B800H:offset], 1, attr+WORD(str[i]));
     INC(x);
     INC(offset); INC(offset);
     INC(i);
   END;
END WrStrAt;



(*==========================================================================*)
(* Following procedure does beep using BIOS service 0EH subfunction 07H.    *)
(*==========================================================================*)

PROCEDURE Beep;
VAR
  r:Registers;
BEGIN
    r.AX:=0E07H;
    Intr(r,10H);
END Beep;

BEGIN
  GetMaxCoords(maxX, maxY);
END Scr.