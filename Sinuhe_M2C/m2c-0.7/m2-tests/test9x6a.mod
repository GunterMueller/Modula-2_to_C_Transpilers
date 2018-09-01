(* p.145 2b-1b *)

(* While statements *)

(* While-expression is a boolean expression. *)

(* i is not a boolean expression             *)

MODULE test9x6a;
 VAR
    i: INTEGER;
 BEGIN
  i:=24;
  WHILE i DO
   DEC(i);
  END;
 END test9x6a.
