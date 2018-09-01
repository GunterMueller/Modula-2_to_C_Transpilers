(* p.146 4b *)

(* For statements *)

(* The  limit must be compatible with the control variable. *)

(* i is not compatible with crd                             *)

MODULE test9x8g;
 VAR
      i: INTEGER;
    crd: CARDINAL;
      p: REAL;
 BEGIN
  p:=2.0;
  i:=22;
  FOR crd:=17 TO i DO
      p:=p/7.8;
  END;
 END test9x8g.
