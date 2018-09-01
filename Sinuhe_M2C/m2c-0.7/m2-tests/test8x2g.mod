(* p.142 12b *)

(* Logical operators *)

(* The operators OR,AND and NOT apply to  *)
(*   BOOLEAN operands.                    *)

(* The wrong operand type for OR operation *)

MODULE test8x2g;
 VAR
    x,y: INTEGER;
 BEGIN
  x:=0;
  y:=1;
  IF x OR y THEN
     x:=1;
  ELSE
     y:=0;
  END;
 END test8x2g.
