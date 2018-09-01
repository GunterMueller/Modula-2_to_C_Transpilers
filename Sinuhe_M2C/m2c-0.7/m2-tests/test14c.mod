(* p.156 16b-15b *)

(* Compilation units *)

(* The definition module is an export list *)

(* The variable  y  is not exported *)

MODULE test14c;
 IMPORT add14c;
 VAR
    z: REAL;
 BEGIN
  z:=add14c.x;
  add14c.y:=z;
 END test14c.
