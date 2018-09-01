(* p.145 17b-16b *)

(* Case statements *)

(* The type of a case expression must not be REAL *)

(* 1.0 is wrong case expression                   *)

MODULE test9x5a;
 VAR
    x: REAL;
    y: CHAR;
 BEGIN
  x:=2.0;
  CASE x OF
   1.0: y:="A";
   ELSE y:="B";
  END;
 END test9x5a.
