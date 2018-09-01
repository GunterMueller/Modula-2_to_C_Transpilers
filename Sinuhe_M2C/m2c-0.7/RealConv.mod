IMPLEMENTATION MODULE RealConv;
FROM ASCII IMPORT nul;
FROM String IMPORT Length,Copy,Append,Insert;
FROM NumConv IMPORT LongIntToStr;

 PROCEDURE LongRealToStr(r:LONGREAL;VAR s:ARRAY OF CHAR);
  CONST Cdigits=20;
  VAR
    i,dig,first0,first9:CARDINAL;exp:INTEGER;flmin:BOOLEAN;
    chstr:ARRAY[0..1] OF CHAR;
    mstr,expstr:ARRAY[0..Cdigits+2] OF CHAR;(* and min,point,nul *)
  BEGIN
   IF r<0.0 THEN flmin:=TRUE;r:=-r;
   ELSE flmin:=FALSE; END;
   exp:=0;
   IF r # 0.0 THEN
    LOOP
     IF r<1.0 THEN r:=r*10.0;DEC(exp);
     ELSIF r>=10.0 THEN r:=r/10.0;INC(exp);
     ELSE EXIT;END;
    END;
   END;
   first9:=0;first0:=0;
   FOR i:=0 TO Cdigits-1 DO
    dig:=TRUNC(r);
    mstr[i]:=CHR(dig+ORD("0"));
    r:=r-FLOAT(dig);r:=r*10.0;
    IF dig # 0 THEN first0:=i+1;END;
    IF dig # 9 THEN first9:=i+1;END;
   END;
   IF first0<2 THEN first0:=2; END;
   IF first9=0 THEN mstr[0]:="1";mstr[1]:="0";INC(exp);
   ELSIF first9 # Cdigits THEN
    mstr[first9]:="0";DEC(first9);mstr[first9]:=CHR(ORD(mstr[first9])+1);
   END;
   IF first9<2 THEN first9:=2;END;
   mstr[first0]:=nul; mstr[first9]:=nul;
   chstr:="."; Insert(chstr,mstr,1);
   IF flmin THEN chstr:="-";Insert(chstr,mstr,0);END;
   LongIntToStr(exp,10,expstr);
   chstr:="E";Insert(chstr,expstr,0);
   IF Length(mstr)+Length(expstr)-1<=CARDINAL(HIGH(s)) THEN
    Copy(s,mstr);Append(s,expstr);
   ELSE RCStatus:=OutOfRange; END;
  END LongRealToStr;

 PROCEDURE StrToLongReal(VAR s:ARRAY OF CHAR;VAR r:LONGREAL);
  CONST Cmaxord=1000;
  VAR
    i,l:CARDINAL;ord,exp,j:INTEGER;flmin,flminord:BOOLEAN;
    res,rbound:LONGREAL;
  BEGIN
   r:=0.0;RCStatus:=BadNumber;l:=Length(s);
   res:=0.0;i:=0;
   WHILE (i<l) & (s[i]=" ") DO INC(i); END;
   IF i>=l THEN RETURN; END;
   flmin:=FALSE;
   IF s[i]="+" THEN INC(i);
   ELSIF s[i]="-" THEN flmin:=TRUE;INC(i); END;
   ord:=0;rbound:=MAX(LONGREAL)/10.0;
   IF (i>=l) OR (s[i]<"0") OR(s[i]>"9") THEN RETURN;END;
   REPEAT
    IF res<rbound THEN res:=res*10.0+FLOAT(ORD(s[i])-ORD("0"));
    ELSE INC(ord); END;
    INC(i);
   UNTIL (i>=l) OR (s[i]<"0") OR (s[i]>"9");
   IF (i<l) & (s[i]=".") THEN
    INC(i);IF (i>=l) OR (s[i]<"0") OR(s[i]>"9") THEN RETURN; END;
    REPEAT
     IF res<rbound THEN
      DEC(ord);res:=res*10.0+FLOAT(ORD(s[i])-ORD("0"));
     END;
     INC(i);
    UNTIL (i>=l) OR (s[i]<"0") OR (s[i]>"9");
   END;
   IF (i<l) & (s[i]="E") THEN
    INC(i); IF i>=l THEN RETURN;END;
    flminord:=FALSE;
    IF s[i]="+" THEN INC(i);
    ELSIF s[i]="-" THEN INC(i);flminord:=TRUE;END;
    IF (i>=l) OR (s[i]<"0") OR (s[i]>"9") THEN RETURN; END;
    exp:=0;
    REPEAT
     IF exp<=Cmaxord THEN j:=ORD(s[i])-ORD("0");exp:=exp*10+j; END;
     INC(i);
    UNTIL (i>=l) OR (s[i]<"0") OR (s[i]>"9");
    IF flminord THEN DEC(ord,exp);ELSE INC(ord,exp); END;
   END;
   IF i<l THEN RETURN; END;
   IF ord>=0 THEN flminord:=FALSE;
   ELSE ord:=-ord;flminord:=TRUE; END;
   FOR j:=1 TO ord DO
    IF flminord THEN res:=res/10.0;
    ELSIF res>=rbound THEN RCStatus:=OutOfRange;RETURN;
    ELSE res:=res*10.0; END;
   END;
   RCStatus:=Done;
   IF flmin THEN r:=-res; ELSE r:=res; END;
  END StrToLongReal;

 PROCEDURE StrToReal(VAR s:ARRAY OF CHAR;VAR r:REAL);
  VAR lr:LONGREAL;
  BEGIN
   StrToLongReal(s,lr);r:=0.0;
   IF RCStatus=Done THEN
    IF (lr<MIN(REAL)) OR (lr>MAX(REAL)) THEN RCStatus:=OutOfRange;
    ELSE r:=lr; END;
   END;
  END StrToReal;

 PROCEDURE StrToShortReal(VAR s:ARRAY OF CHAR;VAR r:SHORTREAL);
  VAR lr:LONGREAL;
  BEGIN
   StrToLongReal(s,lr);r:=0.0;
   IF RCStatus=Done THEN
    IF (lr<MIN(SHORTREAL)) OR (lr>MAX(SHORTREAL)) THEN RCStatus:=OutOfRange;
    ELSE r:=lr; END;
   END;
  END StrToShortReal;

BEGIN
 RCStatus:=Done;
END RealConv.
