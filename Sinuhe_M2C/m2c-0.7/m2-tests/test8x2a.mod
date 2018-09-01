(* p.142 14t-15t *)

(* Arithmetic operators *)

(* The arithmetic operators (except /) apply to operands *)
(*   of type INTEGER,CARDINAL, or subranges thereof.     *)

(* + is not applied to character operands *)

MODULE test8x2a;
 VAR
    x,y:CHAR;
 BEGIN
  x:="A";
  y:="B";
  x:=x+y;
 END test8x2a.
