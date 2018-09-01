(* p.146 4b *)

(* For statements *)

(* The increment must be a constant expression. *)

(* j is not a constant                          *)

MODULE test9x8h;
 VAR
    i,j:INTEGER;
      p: REAL;
 BEGIN
  p:=2.0;
  j:=2;
  FOR i:=1 TO 24 BY j DO
      p:=2.0*p;
  END;
 END test9x8h.
