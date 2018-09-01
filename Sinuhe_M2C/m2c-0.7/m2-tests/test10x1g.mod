(* p.149 19t-20t *)

(* Formal parameters *)

(* The function procedure without parameters has an empty parameter list. *)

(* F() is declared wrong                                                  *)

MODULE test10x1g;
 PROCEDURE F: REAL;
  BEGIN
   RETURN 2.7;
  END F;
 VAR
    t: REAL;
 BEGIN
  t:=F();
 END test10x1g.
