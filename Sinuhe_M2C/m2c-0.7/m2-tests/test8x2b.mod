(* p.142 14t-15t *)

(* Arithmetic operators *)

(* Arithmetic operators (except /) apply to operands of *)
(*   type INTEGER,CARDINAL, or subranges thereof.       *)

(* DIV is not applied to real operands *)

MODULE test8x2b;
 VAR
    x,y,z:REAL;
 BEGIN
  x:=4.0;
  y:=2.0;
  z:=x DIV y;
 END test8x2b.
