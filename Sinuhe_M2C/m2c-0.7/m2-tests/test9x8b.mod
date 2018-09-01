(* p.146 13b *)

(* For statements *)

(* The control variable can not be imported. *)

(* x is imported                             *)

MODULE test9x8b;
 MODULE M1;
  EXPORT x;
  VAR
     x: INTEGER;
  BEGIN
   x:=3;
  END M1;
 MODULE M2;
  IMPORT x;
  VAR
     p: REAL;
  BEGIN
   p:=1.0;
   FOR x:=1 TO 10 DO
       p:=p*2.4+3.3;
   END;
  END M2;
 BEGIN
  ;
 END test9x8b.
