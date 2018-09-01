(* p.136  9b-8b *)

(* Constant declarations *)

(* A constant expression is an expression which can be evaluated  *)
(*   by a mere textual scan without actually executing            *)
(*   the program. Its operands are constants.                     *)

(* R can not be calculated prior to program execution *)

 MODULE test5a;
  VAR L:REAL;
  CONST R=L+8.92;
  BEGIN
   L:=0.1;
  END test5a.
