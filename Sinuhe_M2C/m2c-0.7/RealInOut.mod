IMPLEMENTATION MODULE RealInOut;
FROM ASCII IMPORT nul;
FROM String IMPORT Length;
FROM InOut IMPORT ReadString,WriteString,Write;
IMPORT RealConv;

CONST
 Cbitinchar=8;

 PROCEDURE ReadReal(VAR x:REAL);
  VAR str:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(str);RealConv.StrToReal(str,x);
   Done:=RealConv.RCStatus=RealConv.Done;
  END ReadReal;

 PROCEDURE ReadShortReal(VAR x:SHORTREAL);
  VAR str:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(str);RealConv.StrToShortReal(str,x);
   Done:=RealConv.RCStatus=RealConv.Done;
  END ReadShortReal;

 PROCEDURE ReadLongReal(VAR x:LONGREAL);
  VAR str:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(str);RealConv.StrToLongReal(str,x);
   Done:=RealConv.RCStatus=RealConv.Done;
  END ReadLongReal;

 PROCEDURE WriteReal(x:REAL; n:CARDINAL);
  BEGIN
   WriteLongReal(x,n);
  END WriteReal;

 PROCEDURE WriteLongReal(x:LONGREAL; n:CARDINAL);
  VAR str:ARRAY[0..80] OF CHAR; l:CARDINAL;
  BEGIN
   RealConv.LongRealToStr(x,str);l:=Length(str);
   WHILE n>l DO Write(" ");DEC(n); END;
   WriteString(str);
  END WriteLongReal;

 PROCEDURE WriteRealOct(x:REAL);
  VAR str:ARRAY[0..80] OF CHAR; l,i:CARDINAL;
    union:RECORD
	   CASE :BOOLEAN OF
	FALSE: x:REAL; |
	TRUE : chararr: ARRAY[0..SIZE(REAL)-1] OF CHAR;
	   END;
	  END;
  BEGIN
   union.x:=x; CharArrToOct(union.chararr,str);
   l:=Length(str);
   IF l#0 THEN
    FOR i:=0 TO l-1 DO Write(str[i]); END;
   END;
  END WriteRealOct;

 PROCEDURE WriteShortRealOct(x:SHORTREAL);
  VAR str:ARRAY[0..80] OF CHAR; l,i:CARDINAL;
    union:RECORD
	   CASE :BOOLEAN OF
	FALSE: x:SHORTREAL; |
	TRUE : chararr: ARRAY[0..SIZE(SHORTREAL)-1] OF CHAR;
	   END;
	  END;
  BEGIN
   union.x:=x; CharArrToOct(union.chararr,str);
   l:=Length(str);
   IF l#0 THEN
    FOR i:=0 TO l-1 DO Write(str[i]); END;
   END;
  END WriteShortRealOct;

 PROCEDURE WriteLongRealOct(x:LONGREAL);
  VAR str:ARRAY[0..80] OF CHAR; l,i:CARDINAL;
    union:RECORD
	   CASE :BOOLEAN OF
	FALSE: x:LONGREAL; |
	TRUE : chararr: ARRAY[0..SIZE(LONGREAL)-1] OF CHAR;
	   END;
	  END;
  BEGIN
   union.x:=x; CharArrToOct(union.chararr,str);
   l:=Length(str);
   IF l#0 THEN
    FOR i:=0 TO l-1 DO Write(str[i]); END;
   END;
  END WriteLongRealOct;

 (*-------- internal procedure ------------*)

 PROCEDURE CharArrToOct(VAR input:ARRAY OF CHAR;VAR str:ARRAY OF CHAR);
  VAR noct,predbit,rest,c,i,j:CARDINAL;
  BEGIN
   noct:=(HIGH(input) * Cbitinchar - 1) DIV 3 + 1;str[noct]:=nul;
   predbit:=0; rest:=0;
   FOR i:=CARDINAL(HIGH(input)) TO 0 BY -1 DO
    c:=ORD(input[i]);
    FOR j:=1 TO predbit DO c:=c*2; END;
    c:=c+rest;INC(predbit,Cbitinchar);
    WHILE predbit>=3 DO
     DEC(predbit,3);
     DEC(noct);str[noct]:=CHR(c MOD 8 + ORD("0"));
     c:=c DIV 8;
    END;
    rest:=c;
   END;
   IF noct # 0 THEN str[0]:=CHR(rest+ORD("0")); END;
  END CharArrToOct;

BEGIN
 Done:=TRUE;
END RealInOut.
