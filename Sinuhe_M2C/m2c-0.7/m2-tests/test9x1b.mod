(* p.144 8t-9t *)

(* Assignments *)

(*  The type of the variable must be assignment compatible *)
(*    with the type of the expression.                     *)

(* The type of R is not assignment compatible with         *)
(*   the expression type                                   *)

MODULE test9x1b;
 VAR
    R:REAL;
 BEGIN
  R:=2+4*8;
 END test9x1b.
