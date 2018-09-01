(* p.145 9t *)

(* If statements *)

(* The expression following the symbol IF is of type BOOLEAN .*)

(* TR is not of type BOOLEAN                                  *)

MODULE test9x4a;
 CONST TR=1;
 VAR
    x,y: INTEGER;
 BEGIN
  IF TR THEN
     x:=0; y:=1;
  ELSE
     x:=1; y:=0;
  END;
 END test9x4a.
