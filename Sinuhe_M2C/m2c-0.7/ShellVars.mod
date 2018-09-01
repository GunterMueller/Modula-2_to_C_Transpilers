IMPLEMENTATION MODULE ShellVars;
FROM ASCII IMPORT nul;
FROM SYSTEM IMPORT ADDRESS,TSIZE;
FROM CPROC IMPORT margc,margv,menvp;

%{
extern int m2argc;
extern char **m2argv,**m2envp;
#define margc(v) v=m2argc
#define margv(v) *(char ***)&(v)=m2argv
#define menvp(v) *(char ***)&(v)=m2envp
%}

TYPE PtrToArgs=POINTER TO POINTER TO CHAR;

 PROCEDURE GetNArg():CARDINAL;
   VAR
     argc:CARDINAL;argv,envp:PtrToArgs;
   BEGIN
     margc(argc);
     margv(argv);
     menvp(envp);
     RETURN argc;
   END GetNArg;

 PROCEDURE GetNthArgStr(n:CARDINAL;VAR str: ARRAY OF CHAR);
   VAR
     argc:CARDINAL;argv,envp:PtrToArgs;
   BEGIN
     str[0]:=nul;
     margc(argc);
     margv(argv);
     menvp(envp);
     IF n>=argc THEN RETURN END;
     argv:=PtrToArgs(CARDINAL(argv)+n*TSIZE(ADDRESS));
     GetStr(argv,str);
   END GetNthArgStr;

 PROCEDURE GetNthEnvStr(n:CARDINAL;VAR str: ARRAY OF CHAR);
   VAR
     argc:CARDINAL;argv,envp:PtrToArgs;i:CARDINAL;
   BEGIN
     str[0]:=nul;
     margc(argc);
     margv(argv);
     menvp(envp);
     i:=0;
     LOOP
       IF envp^=NIL THEN RETURN END;
       IF i=n THEN EXIT END;
       envp:=PtrToArgs(CARDINAL(envp)+TSIZE(ADDRESS));
       INC(i);
     END;
     GetStr(envp,str);
   END GetNthEnvStr;

 (*----- internal procedure:---------*)
 PROCEDURE GetStr(p:PtrToArgs;VAR str:ARRAY OF CHAR);
   TYPE PtrToChar=POINTER TO CHAR;
   VAR
     i:CARDINAL; pch: PtrToChar;
   BEGIN
     pch:=PtrToChar(p^); i:=0;
     WHILE (i<=CARDINAL(HIGH(str))) & (pch^#nul) DO
       str[i]:=pch^; pch:=PtrToChar(CARDINAL(pch)+1); INC(i);
     END;
     IF i<=CARDINAL(HIGH(str)) THEN str[i]:=nul END;
   END GetStr;

BEGIN
END ShellVars.
