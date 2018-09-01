(* p.156 14b *)

(* Compilation units *)

(* definition modules imply the use of qualified export. *)

(* x is not qualified                                    *)

MODULE test14d;
 IMPORT add14d;
 BEGIN
  x:=3;
 END test14d.
