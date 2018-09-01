(* p.146 4b-3b *)

(* For statements *)

(* The increment must be of type INTEGER or CARDINAL. *)

(* increment is of type REAL                          *)

MODULE test9x8i;
 VAR
    i: INTEGER;
    p: REAL;
 BEGIN
  p:=2.7;
  FOR i:=1 TO 10 BY 2.0 DO
      p:=p*4.2;
  END;
 END test9x8i.
