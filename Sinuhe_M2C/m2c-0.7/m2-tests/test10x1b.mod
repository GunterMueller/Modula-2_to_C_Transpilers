(* p.149 8t-9t *)

(* Formal parameters *)

(* formal and actual var-pameters must be of the same type. *)

(* x and y are not of the same type                         *)

MODULE test10x1b;
 PROCEDURE P1(VAR x: CARDINAL);
  BEGIN
   INC(x,3);
  END P1;
 VAR
    y: INTEGER;
 BEGIN
  y:=2;
  P1(y);
 END test10x1b.
