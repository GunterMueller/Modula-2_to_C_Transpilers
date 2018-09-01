(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       String (1.1)                              *
 * Version:                 1.2.0                                     *
 * Date:                    November 18, 1997                         *
 * Conformance Test Passed: November 18, 1997                         *
 * Platform:                Every                                     *
 * Requires:                Generic Modula-2 Compiler                 *
 * To do:                   Future implementations should more rely   *
 *                          on direct memory manipulations (BMB       *
 *                          module Mem)                               *
 * Portability:             This implementation is portable!          *
 *                                                                    *
 * Author:                      Tamas Cservenak & Jasmina Vesic       *
 *                              University of Novi Sad, Yugoslavia    *
 * Modifications:               Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE String;
IMPORT Wr;
TYPE
  relation = (less,equal,greater);

VAR
  errorCode: CARDINAL;

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg)
END SetError;

PROCEDURE Length(s : ARRAY OF CHAR ) : CARDINAL;
   (* by Jasmina Vesic, November, 1997 *)
VAR l : CARDINAL;
BEGIN
  l := 0;
  WHILE (l<=HIGH(s)) & (s[l]#0C) DO
    INC(l)
  END;
  RETURN l
END Length;


PROCEDURE Comp(s, p: ARRAY OF CHAR): relation;
   (* by Tamas Cservenak, May, 1996 *)
VAR i : CARDINAL;
BEGIN
  i := 0;
  WHILE (i<=HIGH(s)) & (i<=HIGH(p)) DO
    IF s[i]>p[i] THEN
      RETURN greater
    ELSIF s[i]<p[i] THEN
      RETURN less
    ELSIF (s[i]=0C) THEN (* implicitly s[i]=r[i]! *)
      RETURN equal
    END;
    INC(i);
  END;
  IF i<=HIGH(s) THEN
    IF s[i]=0C THEN RETURN equal ELSE RETURN greater END
  ELSIF i<=HIGH(p) THEN
    IF p[i]=0C THEN RETURN equal ELSE RETURN less END
  ELSE
    RETURN equal
  END
END Comp;

PROCEDURE SubString (s, p: ARRAY OF CHAR; VAR pos: CARDINAL): BOOLEAN;
   (* by Jasmina Vesic, November, 1997 *)
VAR  i,j,k,plen,slen :INTEGER;
BEGIN
  plen := Length(p);
  slen := Length(s);
  i  := 0;
  LOOP
    IF (i = slen)  THEN RETURN FALSE END;
    j := 0;
    k := i;
    LOOP
      IF (j = plen)  THEN
         pos := i;
         RETURN TRUE
      END;
      IF k = slen THEN RETURN FALSE END;
      IF s[k] # p[j] THEN EXIT END;
      INC(j);
      INC(k);
    END;
    INC(i);
  END;
END SubString;


PROCEDURE Caps( VAR s : ARRAY OF CHAR );
   (* by Jasmina Vesic, November, 1997 *)
VAR i : CARDINAL;
BEGIN
  i := 0;
  WHILE s[i] # 0C DO
    s[i] := CAP(s[i]);
    INC(i)
  END
END Caps;

PROCEDURE Append(VAR s: ARRAY OF CHAR; p: ARRAY OF CHAR);
   (* by Tamas Cservenak, May, 1996.
      Modified by Zoran Budimac, July, 1996 *)
VAR i, slen, plen : CARDINAL;
BEGIN
  slen := Length(s);
  plen := Length(p);
  IF slen+plen > HIGH(s)+1 THEN
     SetError(noRoomError, "Append");
  END;
  FOR i := 0 TO plen-1 DO
    IF slen+i > HIGH(s) THEN
      s[slen+i] := 0C;
      RETURN
    END;
    s[slen+i] := p[i]
  END;
  s[slen+plen] := 0C
END Append;

PROCEDURE Copy (VAR s: ARRAY OF CHAR; p: ARRAY OF CHAR);
   (* by Jasmina Vesic, November 1997 *)
VAR i, plen: CARDINAL;
BEGIN
  plen:=Length(p);
  IF plen  > HIGH(s)+1 THEN
     SetError(noRoomError, "Copy");
  END;
  FOR i := 0 TO plen-1  DO
    IF i > HIGH(s)-1 THEN
      s[i] := 0C;
      RETURN
    END;
    s[i] := p[i]
  END;
  s[plen] := 0C
END Copy;


PROCEDURE Insert(VAR s: ARRAY OF CHAR; p: ARRAY OF CHAR; pos: CARDINAL);
   (* by Zoran Budimac, July, 1996 *)
VAR
  i,j,c,l : CARDINAL;
BEGIN
  l := Length(s);
  i := Length(p);
  IF i+l > HIGH(s)+1 THEN
     SetError(noRoomError, "Insert")
  END;
  IF pos > l THEN
     SetError(wrongPositionError, "Insert")
  END;
  c := l;
  IF c < pos THEN pos := c END;
  DEC(c,pos);
  FOR j := c TO 0 BY -1 DO
    IF (j+pos+i <= HIGH(s)) THEN s[j+pos+i] := s[j+pos]; END;
  END;
  j := 0;
  WHILE (j<i) AND (pos+j <= HIGH(s)) DO
    s[pos+j] := p[j];
    INC(j);
  END;
END Insert;

PROCEDURE Delete(VAR s: ARRAY OF CHAR; from, to: CARDINAL);
   (* by Tamas Cservenak, May, 1996 *)
VAR len,i,shft : CARDINAL;
BEGIN
  len := Length(s);
  IF (len=0) OR (len<from) OR (len<to) OR (from>to) THEN
     SetError(wrongPositionError, "Delete");
  END;
  shft := to-from+1;
  FOR i:=from TO len DO (* from 'from' to 'to' INCLUSIVE *)
    IF i+shft<=len THEN
      s[i] := s[i+shft]
    ELSE
      s[i] := 0C
    END
  END
END Delete;

PROCEDURE Concat(VAR s: ARRAY OF CHAR; s1, s2 : ARRAY OF CHAR);
   (* by Jasmina Vesic, Novemeber, 1997 *)
VAR i,j,s1len,s2len: CARDINAL;
BEGIN
  IF String.Length(s1)+String.Length(s2) > HIGH(s)+1 THEN
     SetError(noRoomError, "Concat");
  END;
  s1len:=Length(s1);
  s2len:=Length(s2);
  i := 0;
  WHILE (i < s1len) DO
    IF i>HIGH(s) THEN s[i] := 0C; RETURN END;
    s[i] := s1[i];
    INC(i)
  END;
  j := 0;
  WHILE (j < s2len) DO
    IF i>HIGH(s) THEN s[i] := 0C; RETURN END;
    s[i] := s2[j];
    INC(i);
    INC(j)
  END;
  s[i] := 0C
END Concat;

PROCEDURE Slice(VAR s: ARRAY OF CHAR; r: ARRAY OF CHAR; from, to: CARDINAL);
   (* by Tamas Cservenak, July, 1996. Debugged by Jasmina Vesic. *)
VAR len, i: CARDINAL;
BEGIN
  len := Length(r);
  IF to-from > HIGH(s) THEN
     SetError(noRoomError, "Slice");
  END;
  IF (len=0) OR (len<from) OR (len<to) OR (from>to) THEN
     SetError(wrongPositionError, "Slice");
     s[0] := 0C;
     RETURN

  END;
  FOR i := from TO to DO
    IF i-from>HIGH(s) THEN RETURN END;
    s[i-from] := r[i]
  END;
  s[to-from+1] := 0C
END Slice;

PROCEDURE Equal(s, p: ARRAY OF CHAR ) : BOOLEAN;
   (* by Tamas Cservenak, May, 1996 *)
VAR i : CARDINAL;
BEGIN
  RETURN (Comp(s, p) = equal)
END Equal;

PROCEDURE LEq(s, p: ARRAY OF CHAR): BOOLEAN;
   (* by Tamas Cservenak, May, 1996 *)
BEGIN
  RETURN (Comp(s, p) <= equal)
END LEq;

PROCEDURE GEq(s, p: ARRAY OF CHAR): BOOLEAN;
   (* by Tamas Cservenak, May, 1996 *)
BEGIN
  RETURN (Comp(s, p) >= equal)
END GEq;

PROCEDURE Error() : CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c;
END Error;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** String.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    noRoomError          : Wr.Str('No room in destination string') |
    wrongPositionError : Wr.Str('Invalid position in a string was requested')
  END;
  HALT;
END PrintError;

BEGIN
  errorCode := 0;
  errorHandler := PrintError;
END String.
