(* p.149 20t-21t *)

(* Formal parameters *)

(* The function procedure without parameters is called with empty list *)
(* of actual parameters.                                               *)

(* The using of F is wrong                                             *)

MODULE test10x1h;
 PROCEDURE F(): REAL;
  BEGIN
   RETURN 2.7;
  END F;
 VAR
    t: REAL;
 BEGIN
  t:=(3.0+F)*8.2;
 END test10x1h.
