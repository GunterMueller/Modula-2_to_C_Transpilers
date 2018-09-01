IMPLEMENTATION MODULE String;
FROM ASCII IMPORT nul;

 PROCEDURE Length(VAR str:ARRAY OF CHAR):CARDINAL;
  VAR l:CARDINAL;
  BEGIN
   l:=0;
   WHILE (l<=CARDINAL(HIGH(str))) & (str[l]#nul) DO INC(l); END;
   RETURN l;
  END Length;

 PROCEDURE Copy(VAR s1,s2:ARRAY OF CHAR);
  VAR i:CARDINAL;
  BEGIN
   i:=0;
   WHILE (i<=CARDINAL(HIGH(s1))) & (i<=CARDINAL(HIGH(s2))) & (s2[i] # nul) DO
    s1[i]:=s2[i];INC(i);
   END;
   IF i<= CARDINAL(HIGH(s1)) THEN s1[i]:=nul; END;
  END Copy;

 PROCEDURE Compare(VAR s1,s2:ARRAY OF CHAR):INTEGER;
  VAR i:CARDINAL;flend2:BOOLEAN;
  BEGIN
   i:=0;
   LOOP
    flend2:=(i>CARDINAL(HIGH(s2))) OR (s2[i]=nul);
    IF (i>CARDINAL(HIGH(s1))) OR (s1[i]=nul) THEN
     IF flend2 THEN RETURN 0; ELSE RETURN -1; END;
    ELSIF flend2 OR (s1[i]>s2[i]) THEN RETURN 1;
    ELSIF s1[i]<s2[i] THEN RETURN -1; END;
    INC(i);
   END;
  END Compare;

 PROCEDURE Append(VAR s1,s2:ARRAY OF CHAR);
  VAR i,j:CARDINAL;
  BEGIN
   i:=Length(s1);j:=0;
   WHILE (i<=CARDINAL(HIGH(s1))) & (j<=CARDINAL(HIGH(s2))) & (s2[j]#nul) DO
    s1[i]:=s2[j];INC(i);INC(j);
   END;
   IF i<=CARDINAL(HIGH(s1)) THEN s1[i]:=nul; END;
  END Append;

 PROCEDURE Insert(VAR substr,str:ARRAY OF CHAR;inx:CARDINAL);
  VAR l1,l2,i,j:CARDINAL;
  BEGIN
   l1:=Length(substr);l2:=Length(str);
   j:=l2;
   WHILE j>inx DO
    DEC(j);
    IF j+l1 <= CARDINAL(HIGH(str)) THEN str[j+l1]:=str[j]; END;
   END;
   i:=0;
   WHILE (i<l1) & (j<=CARDINAL(HIGH(str))) DO
    str[j]:=substr[i];INC(j);INC(i);
   END;
   IF l1+l2 <= CARDINAL(HIGH(str)) THEN str[l1+l2]:=nul; END;
  END Insert;

 PROCEDURE Extract(VAR substr,str:ARRAY OF CHAR;inx,len:CARDINAL);
  VAR i,j:CARDINAL;
  BEGIN
   i:=0;j:=0;
   LOOP
    IF (j>=inx+len) OR (j>CARDINAL(HIGH(str))) OR (str[j]=nul)
       OR (i>CARDINAL(HIGH(substr))) THEN
     EXIT;
    END;
    IF j>=inx THEN substr[i]:=str[j];INC(i);END;
    INC(j);
   END;
   IF i<=CARDINAL(HIGH(substr)) THEN substr[i]:=nul; END;
  END Extract;

 PROCEDURE Delete(VAR str:ARRAY OF CHAR;inx,len:CARDINAL);
  VAR i,n:CARDINAL;
  BEGIN
   IF len=0 THEN RETURN; END;
   i:=0;n:=0;
   WHILE (i<=CARDINAL(HIGH(str))) & (str[i]#nul) DO
    IF i>=inx+len THEN str[i-len]:=str[i];INC(n); END;
    INC(i);
   END;
   IF i>inx THEN str[inx+n]:=nul; END;
  END Delete;

 PROCEDURE Pos(VAR str,substr:ARRAY OF CHAR):INTEGER;
  VAR l1,l2,i,j,num:CARDINAL;
  BEGIN
   l1:=Length(str);l2:=Length(substr);
   IF l1>=l2 THEN
    num:=l1-l2;i:=0;
    WHILE i<=num DO
     j:=0;
     LOOP
      IF j>=l2 THEN RETURN i; END;
      IF str[i+j]#substr[j] THEN EXIT; END;
      INC(j);
     END;
     INC(i);
    END;
   END;
   RETURN -1;
  END Pos;

BEGIN
END String.
