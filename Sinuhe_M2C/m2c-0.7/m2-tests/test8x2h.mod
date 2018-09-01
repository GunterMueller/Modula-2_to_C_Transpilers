(* p.142 3b-2b *)

(* Set operators *)

(* The operators +, -, * and / apply to  *)
(*   operands of any set type and yield  *)
(*   a result of the same type.          *)

(* The wrong operand type for + operation *)

MODULE test8x2h;
 VAR
    SetVar: BITSET;
    x: CHAR;
 BEGIN
  x:="D";
  SetVar:={0,2,4,6}+x;
 END test8x2h.
