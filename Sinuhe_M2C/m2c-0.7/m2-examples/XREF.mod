MODULE XREF;
 FROM InOut IMPORT
  Done,EOL,OpenInput,OpenOutput,Read,Write,WriteInt,
  WriteString,WriteLn,CloseInput,CloseOutput;
 FROM Table IMPORT WordLong,Table,overfull,Initiate,Add,Print;

 TYPE Alfa = ARRAY [0..9] OF CHAR;
 CONST N = 45; (*number of keywords*)
 VAR ch: CHAR;
  i,k,l,m,r,linenumb: INTEGER;
  T: Table;
  ident: ARRAY [0..WordLong-1] OF CHAR;
  key: ARRAY [1..N] OF Alfa; nbr:INTEGER;

 PROCEDURE Copy;
 BEGIN Write(ch); Read(ch);
 END Copy;

 PROCEDURE Title;
 BEGIN linenumb := linenumb + 1;
  WriteInt(linenumb,7); Write(" ");
 END Title;

BEGIN
 Initiate(T);
 key[1] := "AND";        key[2] := "ARRAY";
 key[3] := "BEGIN";      key[4] := "BITSET";
 key[5] := "BOOLEAN";    key[6] := "BY";
 key[7] := "CASE";       key[8] := "CARDINAL";
 key[9] := "CHAR";       key[10] := "CONST";
 key[11] := "DIV";       key[12] := "DO";
 key[13] := "ELSE";      key[14] := "ELSIF";
 key[15] := "END";       key[16] := "EXIT";
 key[17] := "EXPORT";    key[18] := "FALSE";
 key[19] := "FOR";       key[20] := "FROM";
 key[21] := "IF";        key[22] := "IMPORT";
 key[23] := "IN";        key[24] := "INTEGER";
 key[25] := "LOOP";      key[26] := "MOD";
 key[27] := "MODULE";    key[28] := "NOT";
 key[29] := "OF";        key[30] := "OR";
 key[31] := "POINTER";   key[32] := "PROCEDURE";
 key[33] := "QUALIFIED"; key[34] := "RECORD";
 key[35] := "REPEAT";    key[36] := "RETURN";
 key[37] := "SET";       key[38] := "THEN";
 key[39] := "TO";        key[40] := "TRUE";
 key[41] := "TYPE";      key[42] := "UNTIL";
 key[43] := "VAR";       key[44] := "WHILE";
 key[45] := "WITH";
 WriteString("mod=");OpenInput("mod");
 IF NOT Done THEN WriteString("file is not opened");WriteLn;HALT END;
 WriteString("xref=");OpenOutput("xref");
 linenumb := 0; Read(ch);
 IF Done THEN Title;
  REPEAT
   IF (CAP(ch) >= "A") & (CAP(ch) <= "Z") THEN
    k := 0;
    REPEAT ident[k] := ch; k := k + 1; Copy;
    UNTIL (ch < "0") OR
      (ch > "9") & (CAP(ch) < "A") OR (CAP(ch) > "Z");
    l := 1; r := N; ident[k] := " ";
    REPEAT m := (l+r) DIV 2; i := 0; (*binary search*)
     WHILE (ident[i]=key[m,i])&(ident[i]>" ") DO
      i := i + 1
     END;
     IF ident[i] <= key[m,i] THEN r := m - 1 END;
     IF ident[i] >= key[m,i] THEN l := m + 1 END;
    UNTIL l > r;
    IF l = r + 1 THEN Add(T,ident,linenumb) END
   ELSIF (ch>= "0") & (ch <= "9") THEN
    REPEAT Copy
    UNTIL ((ch < "0") OR (ch>"9")) & ((ch<"A") OR (ch>"z"))
   ELSIF ch = "(" THEN
    Copy;
    IF ch = "*" THEN (*commentary*)
     nbr:=0;
     LOOP
      REPEAT
       IF ch = EOL THEN
	Copy; Title
       ELSIF ch ="(" THEN Copy;
	IF ch="*" THEN INC(nbr); Copy;END;
       ELSE Copy
       END
      UNTIL ch = "*";
      Copy;
      IF ch =")" THEN
       IF nbr=0 THEN EXIT;ELSE DEC(nbr);END;
      END;
     END;
     Copy
    END
   ELSIF ch= "'" THEN
    REPEAT Copy UNTIL ch = "'";
    Copy
   ELSIF ch = '"' THEN
    REPEAT Copy UNTIL ch= '"';
    Copy
   ELSIF ch = EOL THEN
    Copy;
    IF Done THEN Title END
   ELSE Copy
   END
  UNTIL NOT Done OR (overfull # 0)
 END;
 IF overfull > 0 THEN
  WriteString("table is overfull");
  WriteInt(overfull,6); Write(EOL)
 END;
 Write(35C); Print(T); CloseInput; CloseOutput
END XREF.
