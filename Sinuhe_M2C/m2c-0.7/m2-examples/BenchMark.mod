(* The program works correctly only in Cbreak mode. *)
MODULE BenchMark;
FROM Storage IMPORT ALLOCATE;
FROM SYSTEM IMPORT ADR,ADDRESS;
FROM Terminal IMPORT Read,BusyRead,Write,WriteLn;
FROM InOut IMPORT WriteCard;
FROM Files IMPORT File,Create,Open,Close,ReadNBytes,WriteNBytes,Mode;
FROM MathLib0 IMPORT sin,exp,ln,sqrt;

TYPE
 NodePtr=POINTER TO Node;
 Node=RECORD
       x,y:CARDINAL;
       Next:NodePtr;
      END;

VAR
 adr:ADDRESS;work,nb:CARDINAL;
 A,B,C:ARRAY [0..255] OF CARDINAL;
 M:ARRAY[0..99],[0..99] OF CARDINAL;
 m,n:CARDINAL;
 Head,q:NodePtr;
 Ch,Ch1:CHAR;
 f:File;
 ok:CHAR;

PROCEDURE Test(Ch:CHAR);
 VAR
  i,j,k:CARDINAL;
  R0,R1,R2:REAL;
  P:NodePtr;
 PROCEDURE R;
  BEGIN
  END R;
 PROCEDURE Q(x,y,z,w:CARDINAL);
  BEGIN
  END Q;
 BEGIN
  CASE Ch OF
   "a":k:=20000;
       REPEAT
	k:=k-1
       UNTIL k=0            |
   "A":k:=20000;
       REPEAT
	DEC(k)
       UNTIL k=0            |
   "b":i:=20000;
       WHILE i>0 DO
	i:=i-1
       END                  |
   "B":i:=20000;
       WHILE i>0 DO
	DEC(i)
       END                  |
   "c":FOR i:=1 TO 20000 DO
       END                  |
   "d":j:=0;
       k:=10000;
       REPEAT
	k:=k-1;
	j:=j+1;
	i:=(k*3) DIV (j*5)
       UNTIL k=0            |
   "e":k:=5000;
       R1:=7.28;
       R2:=34.8;
       REPEAT
	k:=k-1;
	R0:=(R1*R2)/(R1+R2)
       UNTIL k=0            |
   "f":k:=500;
       REPEAT
	R0:=sin(0.7);
	R1:=exp(2.0);
	R0:=ln(10.0);
	R1:=sqrt(18.0);
	k:=k-1
       UNTIL k=0            |
   "g":k:=20000;
       i:=0;
       B[0]:=73;
       REPEAT
	A[i]:=B[i];
	B[i]:=A[i];
	k:=k-1
       UNTIL k=0            |
   "h":k:=20000;
       i:=0;
       B[0]:=73;
       REPEAT
	A[i]:=B[i];
	B[i]:=A[i];
	k:=k-1
       UNTIL k=0            |
   "i":FOR i:=0 TO 99 DO
	FOR j:=0 TO 99 DO
	 M[i,j]:=M[j,i]
	END(*FOR*)
       END(*FOR*)           |
   "j":FOR i:=0 TO 99 DO
	FOR j:=0 TO 99 DO
	 M[i,j]:=M[j,i]
	END(*FOR*)
       END(*FOR*)           |
   "k":k:=20000;
       REPEAT
	R;
	k:=k-1
       UNTIL k=0            |
   "l":k:=20000;
       REPEAT
	Q(i,j,k,m);
	k:=k-1
       UNTIL k=0            |
   "m":k:=500;
       REPEAT
	k:=k-1;
	A:=B;
	B:=C;
	C:=A
       UNTIL k=0            |
   "n":k:=500;
       REPEAT
	P:=Head;
	REPEAT
	 P:=P^.Next;
	UNTIL P=NIL;
	k:=k-1
       UNTIL k=0            |
   "o":k:=5000;
       f:=Open("anyfile",RWmode);
       REPEAT
	k:=k-1;
	i:=ReadNBytes(f,adr,2)
       UNTIL k=0;
       Close(f);
  END(*CASE*)
 END Test;

BEGIN
 f:=Create("anyfile",RWmode);
 n:=5000;adr:=ADR(work);
 REPEAT
  n:=n-1;
  nb:=WriteNBytes(f,adr,2)
 UNTIL n=0;
 Close(f);
 Head:=NIL;
 n:=100;
 REPEAT
  q:=Head;
  ALLOCATE(Head,SIZE(Node));
  Head^.Next:=q;
  n:=n-1
 UNTIL n=0;
 Write(">");
 Read(Ch);
 WHILE ("a"<=Ch) & (Ch<="o") OR (Ch="A") OR (Ch="B") DO
  Write(Ch);
  WriteLn;
  n:=0;
  REPEAT
   n:=n+1;
   Test(Ch);
   Write(".");
   IF (n MOD 50)=0 THEN
    WriteLn;
   END;(*IF*)
   BusyRead(ok);
  UNTIL ok#0C;
  WriteCard(n,6);
  WriteLn;
  Write(">");
  Read(Ch)
 END;(*WHILE*)
 Write(14C)
END BenchMark.
