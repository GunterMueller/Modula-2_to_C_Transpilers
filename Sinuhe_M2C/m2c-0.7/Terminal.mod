IMPLEMENTATION MODULE Terminal;
IMPORT ASCII;
FROM SYSTEM IMPORT ADR;
FROM CPROC IMPORT read,write,unblock,block;

%{
#include <fcntl.h>

static int attr;

static unblock(){
 attr=fcntl(0,F_GETFL,0);
 fcntl(0,F_SETFL,attr |
#ifdef FNDELAY
       FNDELAY
#else
       O_NDELAY
#endif
      );
}

static block(){fcntl(0,F_SETFL,attr);}

%}

 CONST
  stdin=0;
  stdout=1;

 VAR
  lastch: CHAR; (* last character read *)
  reread: BOOLEAN; (* TRUE if the last character should be reread *)


 PROCEDURE Read(VAR ch:CHAR);
  BEGIN
   IF reread THEN ch:=lastch; Done:=TRUE
   ELSE
    Done:=read(stdin, ADR(ch), 1)=1;
   END;
   reread:=FALSE; lastch:=ch;
  END Read;

 PROCEDURE BusyRead(VAR ch:CHAR);
  VAR attr:INTEGER;
  BEGIN
   unblock();
   IF read(stdin, ADR(ch), 1)<=0 THEN ch:=0C; END;
   block();
  END BusyRead;

 PROCEDURE ReadAgain;
  BEGIN
   reread:=TRUE;
  END ReadAgain;

 PROCEDURE Write(ch:CHAR);
  BEGIN
    write(stdout, ADR(ch), 1);
  END Write;

 PROCEDURE WriteLn;
  BEGIN
    Write(ASCII.lf);
  END WriteLn;

 PROCEDURE WriteString(s:ARRAY OF CHAR);
  VAR i:CARDINAL;
  BEGIN
   i:=0;
   WHILE (i<=CARDINAL(HIGH(s))) & (s[i] # ASCII.nul) DO
    Write(s[i]);
    INC(i);
   END;
  END WriteString;

BEGIN
 reread:=FALSE; lastch:=" ";
END Terminal.

