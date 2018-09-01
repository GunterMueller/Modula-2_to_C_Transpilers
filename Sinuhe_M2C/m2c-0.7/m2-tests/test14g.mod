(* p.156 7b-6b *)

(* Compilations units *)

(* Opaque export is not applicable to record type. *)

(* The import of RecType is illegal                *)

MODULE test14g;
 IMPORT add14g;
 VAR
    r: add14g.RecType;
 BEGIN
  r.m:=2;
 END test14g.
