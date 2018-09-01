IMPLEMENTATION MODULE MathLib0;
FROM CPROC IMPORT msqrt,mexp,mln,msin,mcos,marctan,mreal;


%{
#include <math.h>
double sin(),cos(),atan(),exp(),log(),sqrt();
#define msqrt(r,x) r=sqrt(x)
#define mexp(r,x) r=exp(x);
#define mln(r,x) r=log(x)
#define msin(r,x) r=sin(x)
#define mcos(r,x) r=cos(x)
#define marctan(r,x) r=atan(x)
#define mreal(r,i) r=i
%}


PROCEDURE sqrt(x:REAL):REAL;
 BEGIN msqrt(x,x);RETURN x;END sqrt;

PROCEDURE longsqrt(x:LONGREAL):LONGREAL;
 BEGIN msqrt(x,x);RETURN x;END longsqrt;

PROCEDURE exp(x:REAL):REAL;
 BEGIN mexp(x,x);RETURN x;END exp;

PROCEDURE longexp(x:LONGREAL):LONGREAL;
 BEGIN mexp(x,x);RETURN x;END longexp;

PROCEDURE ln(x:REAL):REAL;
 BEGIN mln(x,x);RETURN x;END ln;

PROCEDURE longln(x:LONGREAL):LONGREAL;
 BEGIN mln(x,x);RETURN x;END longln;

PROCEDURE sin(x:REAL):REAL;
 BEGIN msin(x,x);RETURN x;END sin;

PROCEDURE longsin(x:LONGREAL):LONGREAL;
 BEGIN msin(x,x);RETURN x;END longsin;

PROCEDURE cos(x:REAL):REAL;
 BEGIN mcos(x,x);RETURN x;END cos;

PROCEDURE longcos(x:LONGREAL):LONGREAL;
 BEGIN mcos(x,x);RETURN x;END longcos;

PROCEDURE arctan(x:REAL):REAL;
 BEGIN marctan(x,x);RETURN x;END arctan;

PROCEDURE longarctan(x:LONGREAL):LONGREAL;
 BEGIN marctan(x,x);RETURN x;END longarctan;

PROCEDURE realconv(x:INTEGER):REAL;
  VAR r:LONGREAL;
 BEGIN mreal(r,x);RETURN r;END realconv;

PROCEDURE longrealconv(x:LONGINT):LONGREAL;
  VAR r:LONGREAL;
 BEGIN mreal(r,x);RETURN r;END longrealconv;

PROCEDURE entier(x:REAL):INTEGER;
 VAR i:INTEGER;flmin:BOOLEAN;
 BEGIN
  IF x<0.0 THEN x:=-x;flmin:=TRUE;
  ELSE flmin:=FALSE;END;
  i:=TRUNC(x+0.5);
  IF flmin THEN i:=-i;END;
 END entier;

PROCEDURE longentier(x:LONGREAL):LONGINT;
 VAR i:LONGINT;flmin:BOOLEAN;
 BEGIN
  IF x<0.0 THEN x:=-x;flmin:=TRUE;
  ELSE flmin:=FALSE;END;
  i:=TRUNC(x+0.5);
  IF flmin THEN i:=-i;END;
 END longentier;

BEGIN
END MathLib0.
