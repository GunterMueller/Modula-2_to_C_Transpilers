(* p.146 13b*)

(* For statements *)

(* The control variable can not be a value parameter. *)

(* x is a value parameter                             *)

MODULE test9x8d;
 PROCEDURE PP(x: INTEGER; VAR p: REAL);
  BEGIN
   p:=2.4;
   FOR x:=1 TO 10 DO
       p:=8.8*p;
   END;
  END PP;
 VAR
    P: REAL;
 BEGIN
  PP(4,P);
 END test9x8d.
