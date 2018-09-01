IMPLEMENTATION MODULE InOut;
FROM ASCII IMPORT nul,bs,lf,del;
FROM String IMPORT Length,Append;
IMPORT NumConv;
IMPORT Terminal;
IMPORT Files;

 VAR
   termIsInput,termIsOutput:BOOLEAN;
   inf,out:Files.File;

 PROCEDURE OpenInput(defext:ARRAY OF CHAR);
  VAR fname:ARRAY[0..80] OF CHAR;
   f:Files.File;
  BEGIN
   GetFName(fname,defext);
   f:=Files.Open(fname,Files.Rmode);
   Done:=Files.status=Files.Done;
   IF Done THEN termIsInput:=FALSE;inf:=f; END;
  END OpenInput;

 PROCEDURE OpenOutput(defext:ARRAY OF CHAR);
  VAR fname:ARRAY[0..80] OF CHAR;
   f:Files.File;
  BEGIN
   GetFName(fname,defext);
   f:=Files.Create(fname,Files.Wmode);
   Done:=Files.status=Files.Done;
   IF Done THEN termIsOutput:=FALSE;out:=f; END;
  END OpenOutput;

 PROCEDURE CloseInput;
  BEGIN
   IF NOT termIsInput THEN Files.Close(inf); END;
   termIsInput:=TRUE;
  END CloseInput;

 PROCEDURE CloseOutput;
  BEGIN
   IF NOT termIsOutput THEN Files.Close(out); END;
   termIsOutput:=TRUE;
  END CloseOutput;

 PROCEDURE Read(VAR ch:CHAR);
  BEGIN
   IF termIsInput THEN
    Terminal.Read(ch);
    IF ch=lf THEN ch:=EOL; END;
    Done:=Terminal.Done;
   ELSE
    Files.ReadChar(inf,ch);Done:=NOT Files.IsEof(inf);
    IF ch=lf THEN ch:=EOL; END;
   END;
  END Read;

 PROCEDURE Write(ch:CHAR);
  BEGIN
   IF termIsOutput THEN
    IF ch=EOL THEN
     Terminal.Write(lf);
    ELSE Terminal.Write(ch); END;
   ELSE
    IF ch=EOL THEN
     Files.WriteChar(out,lf);
    ELSE Files.WriteChar(out,ch); END;
   END;
  END Write;

 PROCEDURE ReadString(VAR s:ARRAY OF CHAR);
  VAR ch:CHAR;i:CARDINAL;flend:BOOLEAN;
  BEGIN
   i:=0;flend:=FALSE;
   REPEAT
    Read(ch);
    IF (ch # " ") OR (i>0)THEN
     IF (ch=del) & termIsInput THEN
      IF i>0 THEN
       DEC(i);Write(bs);Write(" ");Write(bs);
      END;
     ELSIF ch<=" " THEN flend:=TRUE;
     ELSE
      s[i]:=ch;INC(i);flend:=i>CARDINAL(HIGH(s));
     END;
    END;
   UNTIL flend;
   IF i<=CARDINAL(HIGH(s)) THEN s[i]:=nul; END;
   termCh:=ch;
  END ReadString;

 PROCEDURE ReadInt(VAR x:INTEGER);
  VAR s:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(s);NumConv.StrToInt(s,10,x);
   Done:=NumConv.NCStatus=NumConv.Done;
  END ReadInt;

 PROCEDURE ReadShortInt(VAR x:SHORTINT);
  VAR s:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(s);NumConv.StrToShortInt(s,10,x);
   Done:=NumConv.NCStatus=NumConv.Done;
  END ReadShortInt;

 PROCEDURE ReadLongInt(VAR x:LONGINT);
  VAR s:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(s);NumConv.StrToLongInt(s,10,x);
   Done:=NumConv.NCStatus=NumConv.Done;
  END ReadLongInt;

 PROCEDURE ReadCard(VAR x:CARDINAL);
  VAR s:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(s);NumConv.StrToCard(s,10,x);
   Done:=NumConv.NCStatus=NumConv.Done;
  END ReadCard;

 PROCEDURE ReadShortCard(VAR x:SHORTCARD);
  VAR s:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(s);NumConv.StrToShortCard(s,10,x);
   Done:=NumConv.NCStatus=NumConv.Done;
  END ReadShortCard;

 PROCEDURE ReadLongCard(VAR x:LONGCARD);
  VAR s:ARRAY[0..80] OF CHAR;
  BEGIN
   ReadString(s);NumConv.StrToLongCard(s,10,x);
   Done:=NumConv.NCStatus=NumConv.Done;
  END ReadLongCard;

 PROCEDURE WriteLn;
  BEGIN
   Write(EOL);
  END WriteLn;

 PROCEDURE WriteString(s:ARRAY OF CHAR);
  VAR i:CARDINAL;
  BEGIN
   i:=0;
   WHILE (i<=CARDINAL(HIGH(s))) &(s[i]#nul) DO Write(s[i]);INC(i); END;
  END WriteString;

 PROCEDURE WriteInt(x:INTEGER;n:CARDINAL);
  BEGIN
   WriteLongInt(x,n);
  END WriteInt;

 PROCEDURE WriteLongInt(x:LONGINT;n:CARDINAL);
  VAR s:ARRAY[0..80] OF CHAR;i:CARDINAL;
  BEGIN
   NumConv.LongIntToStr(x,10,s);
   IF n#0 THEN
    FOR i:=Length(s) TO n-1 DO Write(" "); END;
   END;
   WriteString(s);
  END WriteLongInt;

 PROCEDURE WriteCard(x,n:CARDINAL);
  BEGIN
   OutLongCard(x,n,10);
  END WriteCard;

 PROCEDURE WriteLongCard(x:LONGCARD;n:CARDINAL);
  BEGIN
   OutLongCard(x,n,10);
  END WriteLongCard;

 PROCEDURE WriteOct(x,n:CARDINAL);
  BEGIN
   OutLongCard(x,n,8);
  END WriteOct;

 PROCEDURE WriteLongOct(x:LONGCARD;n:CARDINAL);
  BEGIN
   OutLongCard(x,n,8);
  END WriteLongOct;

 PROCEDURE WriteHex(x,n:CARDINAL);
  BEGIN
   OutLongCard(x,n,16);
  END WriteHex;

 PROCEDURE WriteLongHex(x:LONGCARD;n:CARDINAL);
  BEGIN
   OutLongCard(x,n,16);
  END WriteLongHex;

 (*------ internal procedures ------------*)

 PROCEDURE GetFName(VAR fn,defext:ARRAY OF CHAR);
  VAR flterm:BOOLEAN;ch:CHAR;i:CARDINAL;
  BEGIN
   flterm:=termIsInput;termIsInput:=TRUE;ch:=termCh;
   ReadString(fn);i:=Length(fn);
   IF (i#0) & (fn[i-1]=".") THEN Append(fn,defext); END;
   termIsInput:=flterm;termCh:=ch;
  END GetFName;

 PROCEDURE OutLongCard(x:LONGCARD;n,base:CARDINAL);
  VAR s:ARRAY[0..80] OF CHAR;i:CARDINAL;
  BEGIN
   NumConv.LongCardToStr(x,base,s);
   IF n#0 THEN
    FOR i:=Length(s) TO n-1 DO Write(" ");END;
   END;
   WriteString(s);
  END OutLongCard;

BEGIN
 termIsInput:=TRUE;termIsOutput:=TRUE;Done:=TRUE;termCh:=" ";
END InOut.
