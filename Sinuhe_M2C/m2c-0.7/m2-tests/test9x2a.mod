(* p.144 14b-13b *)

(* Procedure calls *)

(* In the case of variable parameters, an actual parameter must be *)
(* a designator denoting a variable.                               *)

(* The actual parameter for x is anexpression                      *)

MODULE test9x2a;
 PROCEDURE PR(VAR x:REAL; y: INTEGER);
  BEGIN
   IF y<0 THEN
      x:=0.1;
   ELSE
      x:=0.25;
   END;
  END PR;
 VAR
    t: REAL;
    d: INTEGER;
 BEGIN
  t:=0.2;
  d:=4;
  PR(t+8.4,d);
 END test9x2a.
