IMPLEMENTATION MODULE NumConv;
FROM ASCII IMPORT nul;
FROM String IMPORT Length,Insert;

 VAR digits:ARRAY[0..16] OF CHAR;

 PROCEDURE LongIntToStr(i:LONGINT;b:Base;VAR s:ARRAY OF CHAR);
  VAR sign:ARRAY[0..1]OF CHAR;c:LONGCARD;
  BEGIN
   IF i<0 THEN
    sign:="-";c:=-i;LongCardToStr(c,b,s);
    Insert(sign,s,0);
   ELSE
    LongCardToStr(i,b,s);
   END;
  END LongIntToStr;

 PROCEDURE LongCardToStr(c:LONGCARD;b:Base;VAR s:ARRAY OF CHAR);
  VAR fact:LONGCARD;i:CARDINAL;
  BEGIN
   fact:=1;
   WHILE c DIV fact>=b DO fact:=fact*b; END;
   i:=0;
   REPEAT
    IF i<=CARDINAL(HIGH(s)) THEN s[i]:=digits[c DIV fact]; END;
    INC(i);c:=c MOD fact;fact:=fact DIV b;
   UNTIL fact=0;
   IF i<=CARDINAL(HIGH(s)) THEN s[i]:=nul; END;
   IF i-1<=CARDINAL(HIGH(s)) THEN NCStatus:=Done;
   ELSE NCStatus:=OutOfRange; END;
  END LongCardToStr;

 PROCEDURE StrToLongInt(VAR s:ARRAY OF CHAR;b:Base;VAR i:LONGINT);
  VAR sign:CHAR;num,limin,limax:LONGCARD;
  BEGIN
   StrToNumb(s,b,num,sign);i:=0;
   IF NCStatus=Done THEN
    limax:=MAX(LONGINT);limin:=CARDINAL(-(MIN(LONGINT)+1))+1;
    IF (num>limax) & ((sign # "-") OR (num=limin)) THEN
     NCStatus:=OutOfRange;
    ELSE
     i:=num;IF sign="-" THEN i:=-i; END;
    END;
   END;
  END StrToLongInt;

 PROCEDURE StrToLongCard(VAR s:ARRAY OF CHAR;b:Base;VAR c:LONGCARD);
  VAR sign:CHAR;num:LONGCARD;
  BEGIN
   StrToNumb(s,b,num,sign);c:=0;
   IF NCStatus=Done THEN
    IF sign # " " THEN NCStatus:=BadNumber;
    ELSE c:=num; END;
   END;
  END StrToLongCard;

 PROCEDURE StrToInt(VAR s:ARRAY OF CHAR;b:Base;VAR i:INTEGER);
  VAR l:LONGINT;
  BEGIN
   StrToLongInt(s,b,l);i:=0;
   IF NCStatus=Done THEN
    IF (l<0) & (l<MIN(INTEGER)) OR (l>MAX(INTEGER)) THEN
     NCStatus:=OutOfRange;
    ELSE i:=l; END;
   END;
  END StrToInt;

 PROCEDURE StrToCard(VAR s:ARRAY OF CHAR;b:Base;VAR c:CARDINAL);
  VAR l:LONGCARD;
  BEGIN
   StrToLongCard(s,b,l);c:=0;
   IF NCStatus=Done THEN
    IF l>MAX(CARDINAL) THEN NCStatus:=OutOfRange;
    ELSE c:=l; END;
   END;
  END StrToCard;

 PROCEDURE StrToShortInt(VAR s:ARRAY OF CHAR;b:Base;VAR i:SHORTINT);
  VAR l:LONGINT;
  BEGIN
   StrToLongInt(s,b,l);i:=0;
   IF NCStatus=Done THEN
    IF (l<0) & (l<MIN(SHORTINT)) OR (l>MAX(SHORTINT)) THEN
      NCStatus:=OutOfRange;
    ELSE i:=l; END;
   END;
  END StrToShortInt;

 PROCEDURE StrToShortCard(VAR s:ARRAY OF CHAR;b:Base;VAR c:SHORTCARD);
  VAR l:LONGCARD;
  BEGIN
   StrToLongCard(s,b,l);c:=0;
   IF NCStatus=Done THEN
    IF l>MAX(SHORTCARD) THEN NCStatus:=OutOfRange;
    ELSE c:=l; END;
   END;
  END StrToShortCard;

 (*---------------- internal procedures -----------------*)

 PROCEDURE valof(c:CHAR):CARDINAL;
  BEGIN
   IF (c>="A") & (c<="F") THEN RETURN 10+ORD(c)-ORD("A");
   ELSIF (c>="0") & (c<="9") THEN RETURN ORD(c)-ORD("0");
   ELSE RETURN 100; END;
  END valof;

 PROCEDURE StrToNumb(VAR s:ARRAY OF CHAR;b:Base;
		     VAR num:LONGCARD;VAR sign:CHAR);
  VAR max,last:LONGCARD; l,t,i:CARDINAL;
  BEGIN
   num:=0;NCStatus:=BadNumber;l:=Length(s);i:=0;sign:=" ";
   WHILE (i<l) & (s[i]=" ") DO INC(i); END;
   IF i>=l THEN RETURN; END;
   IF (s[i]="+") OR (s[i]="-")THEN sign:=s[i];INC(i); END;
   IF i>=l THEN RETURN; END;
   max:=MAX(LONGCARD) DIV b;last:=MAX(LONGCARD) MOD max;
   LOOP
    IF i>=l THEN EXIT; END;
    t:=valof(s[i]);
    IF t>=b THEN RETURN;
    ELSE
     IF (num>max) OR (num=max) & (t=last) THEN
      NCStatus:=OutOfRange;RETURN;
     ELSE num:=(num*b)+t;INC(i); END;
    END;
   END;
   NCStatus:=Done;
  END StrToNumb;

 BEGIN
  NCStatus:=Done;digits:="0123456789ABCDEF";
 END NumConv.
