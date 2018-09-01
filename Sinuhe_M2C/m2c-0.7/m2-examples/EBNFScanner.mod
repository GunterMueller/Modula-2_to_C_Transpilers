IMPLEMENTATION MODULE EBNFScanner;
FROM InOut IMPORT EOL,Read,Write,WriteLn,WriteCard;

 VAR ch: CHAR;

 MODULE LineProcessing;
  IMPORT EOL,ch,linenumb,Read,Write,WriteLn,WriteCard;
  EXPORT GetLiter,MarkError,SkipLine;

  CONST LineWidth=100;
  VAR cc: INTEGER; (* index of current liter *)
   cc1: INTEGER; (* nomber of lterals in current line *)
   cc2: INTEGER; (* literals count on line of error *)
   line: ARRAY[0..LineWidth-1] OF CHAR;

  PROCEDURE GetLine;
   BEGIN
    IF cc2>0 THEN
     WriteLn; cc2:=0; (* error line *)
    END;
    Read(ch);
    IF ch = 0C THEN (*end of line*)
     line[0]:=177C; cc1:=1
    ELSE
     linenumb := linenumb + 1;
     WriteCard(linenumb,5);
     Write(" "); cc1:=0;
     LOOP
      Write(ch);
      line[cc1]:=ch; cc1:=cc1+1;
      IF (ch=EOL) OR (ch=0C) THEN EXIT END;
      Read(ch)
     END
    END
   END GetLine;

  PROCEDURE GetLiter;
  BEGIN
   WHILE cc = cc1 DO
    cc:=0; GetLine
   END;
   ch := line[cc]; cc := cc + 1;
  END GetLiter;

  PROCEDURE MarkError(n: INTEGER);
   BEGIN
    IF cc2=0 THEN
     Write("*"); cc2:=3;
     REPEAT Write(" "); cc2:=cc2-1
     UNTIL cc2 = 0;
    END;
    WHILE cc2 < cc DO
     Write(" "); cc2:=cc2+1;
    END;
    Write("^"); WriteCard(n,1); cc2:=cc2 + 2;
   END MarkError;

  PROCEDURE SkipLine;
   BEGIN
    WHILE ch # EOL DO GetLiter END;
    GetLiter
   END SkipLine;

 BEGIN cc:=0; cc1:=0; cc2:=0
 END LineProcessing;

 PROCEDURE GetLexema;
  VAR i:CARDINAL;
  BEGIN
   WHILE ch<=" " DO GetLiter END;
   IF ch = "/" THEN
    SkipLine;
    WHILE ch <= " " DO GetLiter END
   END;
   IF (CAP(ch) <= "Z") & (CAP(ch) >= "A") THEN
    i:=0; lex:=literal;
    REPEAT
     IF i < IdentLong THEN
      id[i]:=ch; i:=i+1
     END;
     IF ch > "Z" THEN lex:=ident; END;
     GetLiter
    UNTIL (CAP(ch) <"A") OR (CAP(ch)>"Z");
    id[i]:=" ";
   ELSIF ch="'" THEN
    i:=0; GetLiter; lex:=literal;
    WHILE ch # "'" DO
     IF i<IdentLong THEN
      id[i]:=ch; i:=i+1
     END;
     GetLiter
    END;
    GetLiter; id[i] := " ";
   ELSIF ch ='"' THEN
    i := 0; GetLiter; lex:=literal;
    WHILE ch # '"' DO
     IF i < IdentLong THEN
      id[i]:=ch; i:=i+1
     END;
     GetLiter
    END;
    GetLiter; id[i] := " ";
   ELSIF ch = "=" THEN lex:= eq; GetLiter
   ELSIF ch = "(" THEN lex:= lbr; GetLiter
   ELSIF ch = ")" THEN lex:= rbr; GetLiter
   ELSIF ch = "[" THEN lex:= lsqbr; GetLiter
   ELSIF ch = "]" THEN lex:= rsqbr; GetLiter
   ELSIF ch = "{" THEN lex:= lfbr; GetLiter
   ELSIF ch = "}" THEN lex:= rfbr; GetLiter
   ELSIF ch = "|" THEN lex:= bar; GetLiter
   ELSIF ch = "." THEN lex:= dot; GetLiter
   ELSIF ch = 177C THEN lex:= other; GetLiter
   ELSE lex:= other; GetLiter
   END
  END GetLexema;

BEGIN
 linenumb := 0; ch := " "
END EBNFScanner.
