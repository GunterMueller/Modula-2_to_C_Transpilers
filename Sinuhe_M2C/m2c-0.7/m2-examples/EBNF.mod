MODULE EBNF;
FROM InOut IMPORT Done,EOL,OpenInput,OpenOutput,Read,Write,WriteLn,
  WriteInt,WriteString,CloseInput,CloseOutput;
FROM EBNFScanner IMPORT Lexema,lex,id,linenumb,GetLexema,MarkError,SkipLine;
FROM Table IMPORT WordLong,Table,overfull,Initiate,Add,Print;

  (* Syntax error codes:
     2 = expectation of ")", 6 = expectation of identifier
     3 = expectation of "]", 7 = expectation of "="
     4 = expectation of "}", 8 = expectation of "."
     5 = expectation of literal, identifier, "(", "[" or "{" *)


VAR T0,T1: Table;
PROCEDURE skip(n: INTEGER);
  (* skip text up to character beginning expression *)
BEGIN MarkError(n);
  WHILE (lex<lbr) OR (lex>dot) DO GetLexema END
END skip;

PROCEDURE SyntExpression;

 PROCEDURE SyntTerm;

  PROCEDURE SyntFactor;
  BEGIN
   IF lex=ident THEN
    Add(T0,id,linenumb); GetLexema
   ELSIF lex=literal THEN
    Add(T1,id,linenumb); GetLexema
   ELSIF lex=lbr THEN
    GetLexema; SyntExpression;
    IF lex=rbr THEN GetLexema
    ELSE skip(2) END
   ELSIF lex=lsqbr THEN
    GetLexema; SyntExpression;
    IF lex=rsqbr THEN GetLexema
    ELSE skip(3) END
   ELSIF lex=lfbr THEN
    GetLexema; SyntExpression;
    IF lex= rfbr THEN GetLexema
    ELSE skip(4) END
   ELSE skip(5) END
  END SyntFactor;

 BEGIN (*SyntTerm*) SyntFactor;
  WHILE lex<bar DO SyntFactor END
 END SyntTerm;

BEGIN (*SyntExpression*)SyntTerm;
 WHILE lex=bar DO GetLexema; SyntTerm END
END SyntExpression;

PROCEDURE SyntRule;
BEGIN (*lex=ident*)
 Add(T0,id,-INTEGER(linenumb));GetLexema;
 IF lex=eq THEN GetLexema
 ELSE skip(7) END;
 SyntExpression;
 IF lex#dot THEN
  MarkError(8); SkipLine END;
 GetLexema
END SyntRule;

BEGIN (*main program*)
 OpenInput("EBNF");
 IF Done THEN
  OpenOutput("XREF");
  Initiate(T0);Initiate(T1);
  GetLexema;
  WHILE (lex=ident) & (overfull=0) DO
   SyntRule
  END;
  IF overfull>0 THEN
   WriteLn; WriteString("Table overflow");
   WriteInt(overfull,6)
  END;
  Write(35C); Print(T0); Print(T1);
  CloseInput; CloseOutput
 END
END EBNF.
