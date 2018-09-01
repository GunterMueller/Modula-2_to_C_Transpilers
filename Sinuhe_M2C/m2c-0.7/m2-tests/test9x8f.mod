(* p.146 4b *)

(* For statements *)

(* The starting value must be compatible with the control variable. *)

(* i is not compatible with crd                                     *)

MODULE test9x8f;
 VAR
      i: INTEGER;
    crd: CARDINAL;
      p: REAL;
 BEGIN
  p:=2.0;
  i:=17;
  FOR crd:=i TO 22 DO
      p:=p/7.8;
  END;
 END test9x8f.
