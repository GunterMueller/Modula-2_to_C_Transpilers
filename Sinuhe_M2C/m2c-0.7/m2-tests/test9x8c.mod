(* p.146 13b *)

(* For statements *)

(* The control variable can not be a variable  parameter. *)

(* x is variable parameter                                *)

MODULE test9x8c;
 PROCEDURE PP(VAR x: INTEGER; VAR p: REAL);
  BEGIN
   p:=2.4;
   FOR x:=1 TO 10 DO
       p:=8.8*p;
   END;
  END PP;
 VAR
    X: INTEGER;
    P: REAL;
 BEGIN
  PP(X,P);
 END test9x8c.
