(* p.142 19t-20t *)

(* Arithmetic operators *)

(* The operators +, -, and * also apply to operands of  *)
(*   type REAL. In this case, both operands must be     *)
(*   of type REAL.                                      *)

(* The wrong type pair of operands for + operation *)

MODULE test8x2d;
 VAR
    RealVar,RealRes: REAL;
    CardVar: CARDINAL;
 BEGIN
  RealVar:=2.25;
  CardVar:=4;
  RealRes:= RealVar+CardVar;
 END test8x2d.
