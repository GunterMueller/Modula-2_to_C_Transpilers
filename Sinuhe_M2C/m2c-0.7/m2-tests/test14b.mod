(* p.156 8t *)

(* Compilation units *)

(* def and imp modules exist in pairs. *)

(* imp module is absent                *)

MODULE test14b;
 IMPORT add14b;
 VAR
    x: REAL;
 BEGIN
  x:=2.4;
  add14b.y:=x;
 END test14b.
