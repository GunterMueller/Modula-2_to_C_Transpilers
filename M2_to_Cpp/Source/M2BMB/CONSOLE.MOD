(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       Console                                   *
 * Version:                 1.10                                      *
 * Date:                    October, 1996                             *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler Version 1.xx   *
 *                                                                    *
 * Author:                  Bojan Skaljak                             *
 *                            University of Novi Sad, Yugoslavia      *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Console;
FROM Lib IMPORT Move;
IMPORT Key, Scr, String, Wr;

IMPORT FileWr, File;

CONST
    MaxColumn=120;
    MaxRow=50;
    MaxScreenSize=MaxColumn*MaxRow*2;
    COLOR=0B800H;
    MONO=0B000H;


TYPE
   ScrollRegion=RECORD
                    x1, y1, x2, y2: CARDINAL;
                END;
   RedrawSection=RECORD
                    screen: ARRAY [1..MaxScreenSize] OF BYTE;
                    cursorX, cursorY: CARDINAL;
                 END;

VAR
   maxX, maxY: CARDINAL;  (* Maximal screen coordinates *)
   currentForeGround, currentBackGround: CARDINAL; (* Current colors *)

   StyleDefinition:CARDINAL;
   ScrRegion:ScrollRegion;
   RedrawData:RedrawSection;
   redrawOn, wrapOn, scrollOn, echoOn, cursorOn: BOOLEAN;


(* Izbaciti ovo?
   VideoMode [0000:0449H]:SHORTCARD;
*)

   errorCode: CARDINAL;

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

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** Console.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError      : Wr.Str('Unknown error');
  | noRoomError       : Wr.Str('No room error');
  | wrongCoordError   : Wr.Str('Wrong coordinate(s)');
  | cannotScroll      : Wr.Str('Scroll option is off');
  | cannotSetStyle    : Wr.Str('Wrong style');
  | cannotSetColor    : Wr.Str('Wrong color');
  | cursorOutOfRegion : Wr.Str('Cursor is out of scroll region');
  | notTextMode       : Wr.Str("It's not text mode");
  END;
  HALT;
END PrintError;

(* ----------------------------------------------------------------- *)

(*===========================================================================*)
(* Help procedure used by output procedures for determining whether cursor is*)
(* or isn't inside the scroll region.                                        *)
(*===========================================================================*)

PROCEDURE CursorInsideRegion(x, y: CARDINAL): BOOLEAN;
VAR
  x1, y1, x2, y2: CARDINAL;
BEGIN
   GetScrollRegion(x1, y1, x2, y2);
   RETURN (x1<=x) AND (x<=x2) AND (y1<=y) AND (y<=y2);
END CursorInsideRegion;


(*=========================================*)
(* Reads character from keyboard with echo *)
(*=========================================*)

PROCEDURE RdChar():CHAR;
   (* Modified by Zoran Budimac, January, 1997 *)
   (* This function now uses Key.RdChar to read the character *)
VAR
  ch : CHAR;
BEGIN
    ch := Key.RdChar();
    IF EchoOn() & (ch >= ' ') THEN (* Ignore special keys *)
       WrChar(ch);
    END;
    RETURN ch;
END RdChar;

(*==========================================================================*)
(* Reads line from standard input using procedure RdChar shown above.       *)
(* If the line is longer then parameter str, excess is ignored and error    *)
(* is set.                                                                  *)
(*==========================================================================*)

PROCEDURE RdLine(VAR str: ARRAY OF CHAR);
CONST
    eol = CHR(13);
    backSpace = CHR(8);
VAR
  i, offset, attrib: CARDINAL;
  x, y: CARDINAL;
  x1, x2, y1, y2: CARDINAL;
  ch: CHAR;
BEGIN
     GetScrollRegion(x1, y1, x2, y2);
     ch := RdChar();
     i:=0;
     WHILE (ch<>eol) DO
          IF ORD(ch)<32 THEN     (* special key is ignored except BackSpace *)
             IF ch=backSpace THEN
                IF i>0 THEN       (* if string is not empty, then calculating *)
                   x:=Scr.CursorX();(* previous position of cursor, so it can *)
                   y:=Scr.CursorY();  (* be erased *)
                   IF CursorInsideRegion(x, y) THEN
                      IF x=x1 THEN
                         IF y>y1 THEN
                            x:=x2;
                            DEC(y);
                         END;
                      ELSE
                         DEC(x);
                      END;
                   ELSIF x=1 THEN
                      IF y>1 THEN
                         x:=CARDINAL(maxX);
                         DEC(y);
                      END;
                   ELSE
                      DEC(x);
                   END;
                   DEC(i);
                END;
                (* returning cursor and erasing character at that position *)
                Scr.WrCharAt(' ', x, y, currentForeGround, currentBackGround);
                Scr.SetCursor(x, y);
             END;
          ELSIF i<=HIGH(str) THEN  (* buffer still have space *)
             str[i]:=ch;
          END;
          IF ORD(ch)>=32 THEN  (* ignore special characters *)
             INC(i);
          END;
          ch:=RdChar();
     END;
     IF i>HIGH(str) THEN
        SetError(noRoomError, "RdLine");
        i:=HIGH(str);
     END;
     str[i]:=CHR(0);   (* terminate string *)
     WrLn;             (* does new line after input *)
END RdLine;

PROCEDURE RdLn;
CONST
    eol=CHR(13);
    backSpace=CHR(8);
VAR
  i,offset,attrib:CARDINAL;
  x,y,x1,x2,y1,y2: CARDINAL;
  ch:CHAR;
BEGIN
     GetScrollRegion(x1, y1, x2, y2);
     ch := RdChar();
     i := 0;
     WHILE (ch<>eol) DO
          IF ORD(ch)<32 THEN     (* special key is ignored except BackSpace *)
             IF ch=backSpace THEN
                IF i>0 THEN       (* if string is not empty, then calculating *)
                   x := Scr.CursorX();(* previous position of cursor, so it can *)
                   y := Scr.CursorY();  (* be erased *)
                   IF CursorInsideRegion(x, y) THEN
                      IF x=x1 THEN
                         IF y>y1 THEN
                            x:=x2;
                            DEC(y);
                         END;
                      ELSE
                         DEC(x);
                      END;
                   ELSIF x=1 THEN
                      IF y>1 THEN
                         x := maxX;
                         DEC(y);
                      END;
                   ELSE
                      DEC(x);
                   END;
                   DEC(i);
                END;
                (* returning cursor and erasing character at that position *)
                Scr.SetCursor(x, y);
                Scr.WrCharAt(' ', x, y, currentForeGround, currentBackGround);
             END;
          END;
          ch:=RdChar();
     END;
     WrLn;             (* does new line after input *)
END RdLn;



(*=========================================================================*)
(* Next two procedures adjusts status of echo and no echo input and output *)
(*=========================================================================*)

PROCEDURE SetEchoOn;
BEGIN
    echoOn := TRUE;
END SetEchoOn;

PROCEDURE SetEchoOff;
BEGIN
    echoOn := FALSE;
END SetEchoOff;

PROCEDURE EchoOn(): BOOLEAN;
BEGIN
    RETURN echoOn;
END EchoOn;

(* xxx: Uraditi *)
PROCEDURE Redraw;
BEGIN
END Redraw;

PROCEDURE SetRedrawOn;
BEGIN
    redrawOn := TRUE;
END SetRedrawOn;

PROCEDURE SetRedrawOff;
BEGIN
    redrawOn := FALSE;
END SetRedrawOff;

PROCEDURE RedrawOn(): BOOLEAN;
BEGIN
    RETURN redrawOn;
END RedrawOn;

(*===========================================================================*)
(* Sets/Gets color which is used for output.                                 *)
(*===========================================================================*)

PROCEDURE SetColorTo(color: ColorType);
BEGIN
    IF color=ColorDefault THEN
       color := Scr.lightGrayColor;
    END;
    IF (color<0) OR (color>15) THEN
        SetError(cannotSetColor, "SetColor");
    ELSE
        currentForeGround:=color;
    END;
END SetColorTo;

PROCEDURE GetColor():ColorType;
BEGIN
    RETURN currentForeGround;
END GetColor;

(*==========================================================================*)
(* Sets/Gets background color.                                             *)
(*==========================================================================*)

PROCEDURE SetBackGround(color:ColorType);
BEGIN
    IF color=ColorDefault THEN
       color := Scr.blackColor;
    END;
    IF (color<0) OR (color>7) THEN
        SetError(cannotSetColor, "SetBackGround");
    ELSE
        currentBackGround:=color;
        (* ClearScreen;   *)
    END;
END SetBackGround;

PROCEDURE GetBackGround():ColorType;
BEGIN
    RETURN currentBackGround;
END GetBackGround;

(*===========================================================================*)
(* Sets/Gets style used for output functions.                                *)
(*===========================================================================*)

PROCEDURE SetStyle(style:StyleType);
BEGIN
    IF (style=StylePlain) OR (style=StyleInverse)
       OR (style=StyleBlink) OR (style=StyleInverseBlink) THEN
       StyleDefinition:=style;
    ELSE
       SetError(cannotSetStyle, "SetStyle");
    END;
END SetStyle;

PROCEDURE GetStyle():StyleType;
BEGIN
    RETURN StyleDefinition;
END GetStyle;

(*===========================================================================*)
(* Defines Scroll Region.                                                    *)
(*===========================================================================*)

PROCEDURE SetScrollRegion(x1,y1,x2,y2: CARDINAL);
VAR
  temp: CARDINAL;
BEGIN
    IF x1>x2 THEN   (* providing that x1,y1 points to top left corner *)
       temp:=x1;    (* and x2,y2 points to down right corner *)
       x1:=x2;
       x2:=temp;
    END;
    IF y1>y2 THEN
       temp:=y1;
       y1:=y2;
       y2:=temp;
    END;
    IF ((x1<0) OR (x1>maxX)) OR ((y1<0) OR (y1>maxY)) OR
       ((x2<0) OR (x2>maxX)) OR ((y2<0) OR (y2>maxY)) THEN
       SetError(wrongCoordError, "SetScrollRegion");
    ELSE
       (*IF x1<>0 THEN*)          (* If any of coordinates is set to 0, that *)
          ScrRegion.x1:=x1;   (* coordinate won't be changed *)
       (*END;*)
       (*IF x2<>0 THEN*)
          ScrRegion.x2:=x2;
       (*END;*)
       (*IF y1<>0 THEN*)
          ScrRegion.y1:=y1;
       (*END;*)
       (*IF y2<>0 THEN*)
          ScrRegion.y2:=y2;
       (*END;*)
    END;
END SetScrollRegion;

PROCEDURE GetScrollRegion(VAR x1,y1,x2,y2: CARDINAL);
BEGIN
    x1:=ScrRegion.x1;
    y1:=ScrRegion.y1;
    x2:=ScrRegion.x2;
    y2:=ScrRegion.y2;
END GetScrollRegion;

(*============================================================================*)
(* Sets/Gets wrapping used for output functions inside of the scroll region.  *)
(*============================================================================*)

PROCEDURE SetWrapOn;
BEGIN
    wrapOn := TRUE;
END SetWrapOn;

PROCEDURE SetWrapOff;
BEGIN
    wrapOn := FALSE;
END SetWrapOff;

PROCEDURE WrapOn(): BOOLEAN;
BEGIN
    RETURN wrapOn;
END WrapOn;

(*===========================================================================*)
(* Sets/Gets scrolling, used by output functions inside of the scroll region.*)
(*===========================================================================*)

PROCEDURE SetScrollOn;
BEGIN
    scrollOn := TRUE;
END SetScrollOn;

PROCEDURE SetScrollOff;
BEGIN
    scrollOn := FALSE;
END SetScrollOff;

PROCEDURE ScrollOn(): BOOLEAN;
BEGIN
    RETURN scrollOn;
END ScrollOn;

(*===========================================================================*)
(* Scroll of the region, defined by SetScrollRegion, is realized by simply   *)
(* moving the part of video memory up or down.Top (bottom) line is filled    *)
(* with spaces (attribute is background+text color).                         *)
(*===========================================================================*)

PROCEDURE ScrollUp;
   (* Modified by Zoran Budimac, January, 1997 *)
   (* This procedure now uses primitives from Scr *)
VAR
  i,x1,y1,x2,y2: CARDINAL;
  RegionWidth,Ofs1,Ofs2,Attrib:CARDINAL;
BEGIN
    IF ScrollOn() THEN
       GetScrollRegion(x1,y1,x2,y2);
       Scr.CopyBlock(x1, y1+1, x2-x1+1, y2-y1, x1, y1);
       Scr.FillBlock(' ', x1, y2, x2-x1+1, 1, currentForeGround, currentBackGround);
    ELSE
       SetError(cannotScroll, "ScrollUp");
    END;
END ScrollUp;

PROCEDURE ScrollDown;
   (* Modified by Zoran Budimac, January, 1997 *)
   (* This procedure now uses primitives from Scr *)
VAR
  x1,y1,x2,y2:CARDINAL;
  i:INTEGER;
  RegionWidth,Ofs1,Ofs2,Attrib:CARDINAL;
BEGIN
    IF ScrollOn() THEN
       GetScrollRegion(x1, y1, x2, y2);
       Scr.CopyBlock(x1, y1, x2-x1+1, y2-y1, x1, y1+1);
       Scr.FillBlock(' ', x1, y1, x2-x1+1, 1, currentForeGround, currentBackGround);
    ELSE
       SetError(cannotScroll, "ScrollDown");
    END;
END ScrollDown;

(*===========================================================================*)
(* Next procedures clears display screen or region.                         *)
(*===========================================================================*)

PROCEDURE ClearScreen;
   (* Clears entire screen, filling it with 'foreground' color
      on 'background' *)
VAR
  maxX, maxY: CARDINAL;
  attr: WORD;
BEGIN
    Scr.Fill(' ', currentForeGround, currentBackGround);
END ClearScreen;

PROCEDURE ClearScrollRegion;
VAR
  x1,y1,x2,y2,i:CARDINAL;
  RegionWidth,Offset:CARDINAL;
  Attr:WORD;
BEGIN
    GetScrollRegion(x1,y1,x2,y2);
    Scr.FillBlock(' ', x1, y1, x2-x1+1, y2-y1+1, currentBackGround, currentForeGround);
END ClearScrollRegion;

(*===========================================================================*)
(* Output procedures, implemented by direct writing into VIDEO memory.       *)
(* WrChar writes character and attribute which is defined with background    *)
(* color, text color and current style in use.                               *)
(*===========================================================================*)

PROCEDURE Test(x, y: CARDINAL; str: ARRAY OF CHAR);
VAR
  f: File.T; (* DEBUG *)
BEGIN
       f := File.StdOut();
       IF (x > 80) OR (y > 25) THEN
         FileWr.Ln(f);
         FileWr.Str(f, str );
         FileWr.Str(f, 'x, y coordinata .');
         FileWr.Card(f, x, 0);
         FileWr.Str(f, '.');
         FileWr.Card(f, y, 0);
         FileWr.Str(f, '.');
         FileWr.Ln(f);
         HALT;
       END;
END Test;

PROCEDURE WrChar(c:CHAR);
VAR
  style:StyleType;
  Temp,TxtColor,BackColor:ColorType;
  x1,y1,x2,y2,minX,x,y:CARDINAL;
  Offset,Attrib:CARDINAL;
BEGIN

       style:=GetStyle();
       BackColor:=currentBackGround;
       TxtColor:=currentForeGround;
       IF (style=StyleInverse) OR (style=StyleInverseBlink) THEN
                               (* inverse output, inverse colors *)
          Temp:=TxtColor;
          TxtColor:=BackColor;
          BackColor:=Temp MOD 8; (* background color can be in range 0-7 *)
          IF style=StyleInverse THEN
             style:=StylePlain;
          ELSE
             style:=StyleBlink;  (* if style is Inverse & Blink *)
          END;
       END;
       x := Scr.CursorX();
       y := Scr.CursorY();
       Test(x, y, '1');
       IF NOT CursorInsideRegion(x,y) THEN
         GetScrollRegion(x, y, x1, y1);
         Scr.SetCursor(x, y);
         Test(x, y, '2');
         x := Scr.CursorX();
         y := Scr.CursorY();
         Test(x, y, '3');
       END;

       Scr.WrCharAt(c, x, y, currentForeGround, currentBackGround);

       x := Scr.CursorX();
       y := Scr.CursorY();
       Test(x, y, '4');
       (* ??? Treba josx negde uglaviti style *)
       (* now, checking if cursor have reached right edge of scroll region
          or screen. *)
       IF CursorInsideRegion(x,y) THEN
          GetScrollRegion(x1, y1, x2, y2);
          maxX := x2;
       END;
       IF x=CARDINAL(maxX) THEN   (* x coordinate reached right edge *)
          WrLn;
       ELSE
          INC(x);
          Test(x, y, '5');
          Scr.SetCursor(x, y);
          Test(x, y, '6');
          x := Scr.CursorX();
          y := Scr.CursorY();
          Test(x, y, '7');
       END;
END WrChar;

(*===========================================================================*)
(* Writes ASCIIZ string.Procedure controls wrapping, while scroll is         *)
(* controlled by WrChar.                                                     *)
(*===========================================================================*)

PROCEDURE WrStr(str:ARRAY OF CHAR);
VAR
  i,Length:CARDINAL;
  x,y,x1,y1,x2,y2:CARDINAL;
  Finished,RedrawStatusChanged:BOOLEAN;

BEGIN
    IF RedrawOn() THEN
       SetRedrawOff;        (* ensure that WrChar don't save *)
       RedrawStatusChanged:=TRUE; (* screen while writing string. *)
    ELSE
       RedrawStatusChanged:=FALSE;
    END;
    Length:=String.Length(str);
    GetScrollRegion(x1, y1, x2, y2);
    i:=0;
    Finished:=FALSE;
    WHILE NOT Finished & (i<Length) DO
          WrChar(str[i]);
          INC(i);
          x:=Scr.CursorX();
          y:=Scr.CursorY();

          IF (CursorInsideRegion(x, y) & (x=x2) & (~WrapOn())) OR
             (NOT CursorInsideRegion(x, y) &
             (x=CARDINAL(maxX))) THEN
                Finished:=TRUE;        (* controls wrapping *)
                WrChar(str[i]);
          END;
    END;
    IF RedrawStatusChanged THEN  (* restoration of RedrawStatus *)
       SetRedrawOn;
    END;
END WrStr;

(*============================================================================*)
(* Skips to new line.In scroll region x coordinate is set to left edge of     *)
(* region, while outside, x coordinate is set to minimum screen coordinate.   *)
(*============================================================================*)

PROCEDURE WrLn;
VAR
  x1,y1,x2,y2,x,y:CARDINAL;
  Offset:CARDINAL;
  f: File.T;
BEGIN
    x:= Scr.CursorX();
    y:= Scr.CursorY();
    IF CursorInsideRegion(x, y) THEN
       GetScrollRegion(x1, y1, x2, y2);
       IF y=y2 THEN               (* take care for scrolling *)
          IF ScrollOn() THEN        (* the scroll region *)
             ScrollUp;
             x:=x1;
          END;
       ELSE                       (* inside scroll region, x coordinate of *)
          x:=x1;               (* the new line is left region x coordinate*)
          INC(y);
       END;
    ELSIF y<maxY THEN             (* outside the scroll region, *)
          x:=0;                   (* goes to new line only if *)
          INC(y);                 (* cursor didn't reach down right *)
    END;                             (* corner of the screen, otherwise cursor*)
    Scr.SetCursor(x, y);           (* don't move *)
END WrLn;

(*===========================================================================*)
(* Procedures used to manipulate with scroll region.Implemented using scroll *)
(* procedures and direct writing to video memory.They does whatever in scroll*)
(* region.If cursor is outside the scroll region error is set.               *)
(*===========================================================================*)

PROCEDURE DeleteLine;
VAR
  x1,y1,x2,y2,x,y:CARDINAL;
  ScrollStatusChanged:BOOLEAN;
BEGIN
    x:= Scr.CursorX();
    y:= Scr.CursorY();
    IF NOT CursorInsideRegion(x,y) THEN
       SetError(cursorOutOfRegion, "DeleteLine");
    ELSE
       GetScrollRegion(x1,y1,x2,y2);
       SetScrollRegion(x1,y,x2,y2);
       IF ~ScrollOn() THEN           (* using scroll up, already implemented *)
          SetScrollOn;
          ScrollStatusChanged:=TRUE;
       ELSE
          ScrollStatusChanged:=FALSE;
       END;
       ScrollUp;
       SetScrollRegion(x1,y1,x2,y2);  (* restoration of scroll region *)
       IF ScrollStatusChanged THEN    (* restoration of scroll status *)
          SetScrollOff;
       END;
    END;
END DeleteLine;

(*===========================================================================*)
(* Inserts line at cursor position.                                          *)
(*===========================================================================*)

PROCEDURE InsertLine;
VAR
  x,y,x1,y1,x2,y2: CARDINAL;

BEGIN
    x := Scr.CursorX();
    y := Scr.CursorY();
    IF CursorInsideRegion(x,y) THEN
       GetScrollRegion(x1,y1,x2,y2);
       IF (y>y1) & ScrollOn() THEN
          SetScrollRegion(x1,y,x2,y2);
          ScrollDown;
       ELSIF ~ScrollOn() THEN                (* if scroll isn't allowed then *)
             SetScrollRegion(x1,y,x2,y2);  (* insert is realized by simply *)
             SetScrollOn;                        (* scrolling down the region *)
             ScrollDown;
             SetScrollOff;
       END;
       SetScrollRegion(x1,y1,x2,y2);
    ELSE
       SetError(cursorOutOfRegion, "InsertLine");
    END;
END InsertLine;

(*===========================================================================*)
(* Clears current line from place of cursor to the end.                      *)
(*===========================================================================*)

PROCEDURE ClearToEOL;
VAR
  x,y,x1,y1,x2,y2: CARDINAL;
BEGIN
    x := Scr.CursorX();
    y := Scr.CursorY();
    IF NOT CursorInsideRegion(x,y) THEN
       SetError(cursorOutOfRegion, "ClearToEOL");
    ELSE
       GetScrollRegion(x1,y1,x2,y2);
       Scr.FillBlock(' ', x1+x+1, y1+y, x2-x+1, 1, currentForeGround, currentBackGround);
     END;
END ClearToEOL;

(*============================================================================*)
(* Clears part of the scroll region, from cursor position to the end of scroll*)
(* region.                                                                    *)
(*============================================================================*)

PROCEDURE ClearToEOD;
VAR
  i, x, y, x1, y1, x2, y2:CARDINAL;
  BackColor: ColorType;
  Attrib, Offset, RegionWidth:CARDINAL;
BEGIN
    x := Scr.CursorX();
    y := Scr.CursorY();
    IF NOT CursorInsideRegion(x, y) THEN
       SetError(cursorOutOfRegion, "ClearToEOD");
    ELSE
       GetScrollRegion(x1,y1,x2,y2);
       ClearToEOL; (* first clears to the end of line, then the rest of region *)
       Scr.FillBlock(' ', x1, y+1, x2-x1, y2-y1, currentForeGround, currentBackGround);
     END;
END ClearToEOD;

(*============================================================================*)
(* Deletes n characters from place of cursor, pushing rest of the line to the *)
(* left.                                                                      *)
(*============================================================================*)

PROCEDURE DeleteChars(n:CARDINAL);
VAR
  x1,y1,x2,y2,x,y:CARDINAL;
BEGIN
    x:= Scr.CursorX();
    y:= Scr.CursorY();
    IF NOT CursorInsideRegion(x,y) THEN
       SetError(cursorOutOfRegion, "DeleteChars");
    ELSE
       GetScrollRegion(x1,y1,x2,y2);
       IF x + n - 1>= x2 THEN
         n := x2 - x + 1;
         Scr.FillBlock(' ', x, y, x2-x+1, 1, currentForeGround, currentBackGround);
       ELSE
         Scr.CopyBlock(x+n, y, n, 1, x, y);
         Scr.FillBlock(' ', x+n, y, x2-x-n+1, 1, currentForeGround, currentBackGround);
       END;
    END;
END DeleteChars;

(*============================================================================*)
(* Insert n characters from place of cursor.Different cases are taken in      *)
(* consideration (scroll,wrap on or off) and appropriate action is taken.     *)
(*============================================================================*)

PROCEDURE InsertChars(n:CARDINAL);
VAR
  x1,y1,x2,y2,x,y:CARDINAL;
BEGIN
    x:= Scr.CursorX();
    y:= Scr.CursorY();
    IF NOT CursorInsideRegion(x,y) THEN
       SetError(cursorOutOfRegion, "DeleteChars");
    ELSE
       GetScrollRegion(x1,y1,x2,y2);
       IF x + n - 1>= x2 THEN
         n := x2 - x + 1;
         Scr.FillBlock(' ', x, y, x2-x+1, 1, currentForeGround, currentBackGround);
       ELSE
         Scr.CopyBlock(x, y, n, 1, x+n, y);
         Scr.FillBlock(' ', x+n, y, x2-x-n+1, 1, currentForeGround, currentBackGround);
       END;
    END;
END InsertChars;


(*===========================  SETTING DEFAULTS  =============================*)

BEGIN
    Scr.GetMaxCoords(maxX,maxY);
    SetScrollRegion(0,0,maxX,maxY);
    Scr.SetCursorOn;
    SetScrollOn;
    SetWrapOn;
    SetEchoOn;
    SetRedrawOff;
    SetStyle(StylePlain);
    SetColorTo(ColorDefault);
    SetBackGround(ColorDefault);
END Console.
