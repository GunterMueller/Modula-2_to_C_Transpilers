(* p.146 17t-18t *)

(* Repeat statements *)

(* Repeat-expression is a BOOLEAN expression. *)

(* i is not a boolean expression              *)

MODULE test9x7a;
 VAR
    i: INTEGER;
 BEGIN
  i:=24;
  REPEAT
   DEC(i);
  UNTIL i
 END test9x7a.
