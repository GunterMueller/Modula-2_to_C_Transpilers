(* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'Fmt' module. It contains operations for conversion
    of real numbers. Operations are characteristic for internal
    representation of real numbers of Top Speed Modula-2 ver. 1.x,
    or 80x86/MS-DOS platform in general.

    These operations are extracted from Top Speed's module Str.
    Some of them are slighly changed to include overflow detection
    and to better conform to BMB Conformance tests.
    ------------------------------------------------------------------ *)

(* Copyright (C) 1987 Jensen & Partners International *)
(*$R-,S-,I-,V-,O-*)
IMPLEMENTATION MODULE TS1RealConvs;
IMPORT String;  (* BMB String operations *)
FROM MATHLIB IMPORT PackedBcd,Log10,Pow,LongToBcd;

CONST
  StrictRealConv = FALSE ;

(*$V+*)

(*$R-,S-,I-*)

TYPE
  ConvIntType = ARRAY ['0'..'F'] OF SHORTCARD;
  BA          = ARRAY[0..1] OF SHORTCARD;

CONST
  ConvStr = '0123456789ABCDEF';
  ConvInt = ConvIntType( 0,1,2,3,4,5,6,7,8,9,255,255,255,255,255,255,255,10,11,12,13,14,15 );
  Div     = BA(0,4);


VAR FloatUse : BOOLEAN;

PROCEDURE Reverse(VAR s: ARRAY OF CHAR; l,h: CARDINAL);
VAR T : CHAR;
BEGIN
  WHILE l < h DO
    T := s[l];
    s[l] := s[h];
    s[h] := T;
    INC(l);
    DEC(h);
  END;
END Reverse;


PROCEDURE CheckBase(VAR b: CARDINAL);
BEGIN
  IF b < 2  THEN b := 2; END;
  IF b > 16 THEN b := 16; END;
END CheckBase;

(*$V-*)

PROCEDURE IntToStr(V: LONGINT;VAR S: ARRAY OF CHAR; Base: CARDINAL; VAR OK: BOOLEAN);
VAR
  i,l : CARDINAL;
  b   : LONGCARD;
BEGIN
  OK := TRUE;
  l  := HIGH(S);
  CheckBase( Base );
  b := VAL( LONGCARD,Base );
  IF V < 0 THEN
    S[0] := '-';
    i := 1;
    V := -V;
  ELSIF FloatUse THEN
    S[0] := '+';
    i := 1;
  ELSE
    i := 0;
  END;

  LOOP
    IF i > l THEN OK := FALSE; EXIT; END;
    S[i] := ConvStr[CARDINAL( LONGCARD(V) MOD b )];
    INC(i);
    V := LONGCARD(V) DIV b;
    IF V = 0 THEN EXIT END;
  END;
  IF i <= l THEN S[i] := CHR(0); END;
  IF S[0] < '0' THEN
    Reverse( S,1,i-1 );
  ELSE
    Reverse( S,0,i-1 );
  END;
END IntToStr;

PROCEDURE StrToReal(s: ARRAY OF CHAR; VAR ok, over: BOOLEAN): REAL;
BEGIN
   RETURN REAL(StrToLongReal(s, ok, over))
END StrToReal;

PROCEDURE StrToLongReal(S: ARRAY OF CHAR; VAR OK, over: BOOLEAN): LONGREAL;
VAR
  c,expsign : CHAR;
  exp,after : INTEGER;
  i         : CARDINAL;
  res,p10   : LONGREAL;
  Neg       : BOOLEAN;
CONST
  Zero = 0.0;
BEGIN
  over := FALSE;   (* ZJB, July 11, 1996 *)
  OK  := TRUE;
  c   := S[0];
  Neg := FALSE;
  i := 1;
  IF c = '-' THEN
    i := 1; Neg := TRUE;
  ELSIF (c >= '0') AND (c <= '9') THEN
    i := 0
  ELSE
    OK := FALSE;
    RETURN Zero;
  END;
  res := 0.0;
  c := S[i];
  WHILE (c <> '.') AND (c >= '0') AND (c <= '9')DO
    IF ((c > '9') OR (c < '0')) THEN
      IF (* StrictRealConv *) ( c = 0C ) AND (i = 0) THEN (* ZJB, July 11 *)
        OK := FALSE; RETURN Zero;
      ELSE
        c := '.' ; DEC(i) ;
      END ;
    ELSE
      IF res <= (MAX(LONGREAL) / 10.0) -   (* ZJB, July 11, 1996 *)
                 VAL( LONGREAL, SHORTCARD(c) - SHORTCARD('0') ) THEN
         res := res * 10.0 + VAL( LONGREAL, ORD(c) - ORD('0') )
      ELSE
         over := TRUE;
         RETURN Zero
      END;
      INC(i);
      c := S[i];
    END ;
  END;
  after := 0;
  IF c = '.' THEN
     INC(i);
     c := S[i]
  END;
  WHILE ( c <> CHAR(0) ) AND ( CAP(c) <> 'E' ) AND (c >= '0') AND (c <= '9') DO
    IF ((c < '0') OR (c > '9')) THEN OK := FALSE; RETURN Zero END;
    IF res <= (MAX(LONGREAL) / 10.0) -   (* ZJB, July 11, 1996 *)
               VAL( LONGREAL, SHORTCARD(c) - SHORTCARD('0') ) THEN
       res := res * 10.0 + VAL( LONGREAL, SHORTCARD(c) - SHORTCARD('0') )
    ELSE
       over := TRUE;
       RETURN Zero
    END;
    INC(i);
    INC(after);
    c := S[i];
  END;
  IF CAP(c) = 'E' THEN
    INC(i);
    expsign := S[i];
    IF expsign = '+' THEN
      INC(i)
    ELSIF expsign = '-' THEN
      INC(i)
    END;
    c := S[i];
    exp := 0;
    WHILE c <> CHAR(0) DO
      IF ( c > '9' ) OR ( c < '0' ) THEN OK := FALSE; RETURN Zero; END;
      IF exp <= (MAX(INTEGER) DIV 10) -   (* ZJB, July 11, 1996 *)
                INTEGER(SHORTCARD(c) - SHORTCARD('0')) THEN
         exp := exp*8 + exp*2 + INTEGER( SHORTCARD(c) - SHORTCARD('0') )
      ELSE
         over := TRUE;
         RETURN Zero
      END;
      INC(i);
      c := S[i];
    END;
    IF expsign = '-' THEN exp := -exp END;
  ELSE
    exp := 0;
  END;
  exp := exp - after;
  p10 := 1.0;
  FOR i := 1 TO ABS(exp) DO
    IF p10 <= (MAX(LONGREAL) / 10.0) THEN   (* ZJB, July 11, 1996 *)
       p10 := p10 * 10.0;
    ELSE
       over := TRUE;
       RETURN Zero
    END
  END;
  IF Neg THEN res := - res END;
  IF exp < 0 THEN
    RETURN res / p10
  ELSE
    IF res <= (MAX(LONGREAL) / p10) THEN   (* ZJB, July 11, 1996 *)
       RETURN res * p10
    ELSE
       over := TRUE;
       RETURN Zero
    END
  END;
END StrToLongReal;

PROCEDURE RealToStr1(V: LONGREAL; Precision: CARDINAL; Eng: BOOLEAN;
                    VAR S: ARRAY OF CHAR; VAR OK: BOOLEAN);
VAR
  X        : PackedBcd;
  i,j,l    : CARDINAL;
  r,t      : LONGREAL;
  Exp,m    : INTEGER;
  Str      : ARRAY[0..7] OF CHAR;
  tb,
  FirstTime: BOOLEAN;

BEGIN
  OK := TRUE;
  l  := HIGH( S );
  IF Precision = 0 THEN
    Precision := 1;
  ELSIF Precision > 17 THEN
    Precision := 17;
  END;
  FirstTime := TRUE;

  IF V # 0.0 THEN
    t := Log10( ABS( V ) );
  ELSE
    t := 1.0;
  END;

  Exp := TRUNC( t );
  LOOP
    m := 1;
    IF Eng THEN
      IF (ABS(V) < 1.0) THEN
        DEC(m,ABS(Exp) MOD 3 );
        IF m < 1 THEN INC(m,3); END;
      ELSE
        INC(m,Exp MOD 3 );
      END;
    END;

    X := LongToBcd( V*Pow( 10.0,VAL( LONGREAL,INTEGER(Precision)-Exp-1)) );
    j := 0;
    IF NOT FirstTime THEN
      EXIT;
    ELSIF (X[Precision DIV 2] >> Div[Precision MOD 2 ] ) MOD 16 # 0 THEN
      INC( Exp );
      FirstTime := FALSE;
    ELSIF (X[(Precision-1) DIV 2] >> Div[(Precision-1) MOD 2 ] ) MOD 16 = 0 THEN
      DEC( Exp );
      FirstTime := FALSE;
    ELSE
      EXIT;
    END;

  END;
  IF X[9]=80H THEN
    S[0] := '-';
  ELSE
    S[0] := ' ';
  END;
  INC(j);

  FOR i := Precision-1 TO 0 BY -1 DO
    IF j > l THEN OK := FALSE; RETURN; END;

    S[j] := CHAR( SHORTCARD('0') + (X[i DIV 2] >> Div[i MOD 2 ] ) MOD 16 );
    INC( j );
    IF i = Precision-CARDINAL(m) THEN
      IF j > l THEN OK := FALSE; RETURN; END;
      S[j] := '.';
      INC(j);
    END;
  END;

  IF j > l THEN OK := FALSE; RETURN; END;
  S[j] := 'E';
  INC( j );
  IF j <= l THEN S[j] := CHR(0); END;

  tb := FloatUse;
  FloatUse := TRUE;

  IntToStr( VAL( LONGINT,Exp-m+1 ),Str,10,OK );

  FloatUse := tb;

  IF ( String.Length( Str ) + j )-1 > l THEN OK := FALSE; RETURN; END;
  String.Append( S,Str );
END RealToStr1;


PROCEDURE RealToStr(VAR s: ARRAY OF CHAR; x: REAL; VAR ok: BOOLEAN);
   (* Interface to BMB implementation. By Zoran Budimac, July 1996 *)
BEGIN
   RealToStr1(LONGREAL(x), 100, FALSE, s, ok)
      (* As much decimals as possible, in scientific notation *)
END RealToStr;

PROCEDURE LongRealToStr(VAR s: ARRAY OF CHAR; x: LONGREAL; VAR ok: BOOLEAN);
   (* Interface to BMB implementation. By Zoran Budimac, July 1996 *)
BEGIN
   RealToStr1(x, 100, FALSE, s, ok)
      (* As much decimals as possible, in scientific notation *)
END LongRealToStr;


BEGIN
  FloatUse := FALSE;
END TS1RealConvs.
