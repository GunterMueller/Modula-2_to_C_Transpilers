(* p.142 15t-18t *)

(* Arithmetic operators *)

(* The operands of + operation must be either of   *)
(*   the same type or the subranges of the same    *)
(*   type.                                         *)

(* The wrong type pair of operands for + operation   *)
(*   inside relation.                                *)

MODULE test8x2c;
 VAR
    IntVar: INTEGER;
    CardVar1,CardVar2: CARDINAL;
 BEGIN
  CardVar1:=2;
  IntVar:=1;
  IF (CardVar1+IntVar)>4 THEN
     CardVar2:=0;
  ELSE
     CardVar2:=1;
  END;
 END test8x2c.
