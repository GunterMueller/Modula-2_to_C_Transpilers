(* p.145 9t *)

(* If statements *)

(* The expression following the symbol ELSIF is of type BOOLEAN. *)

(* TR is not of type BOOLEAN                                     *)

MODULE test9x4b;
 CONST TR=1;
 VAR
    False: BOOLEAN;
    x: CARDINAL;
 BEGIN
  False:=FALSE;
  IF False THEN x:=0;
  ELSIF TR THEN x:=1;
  END;
 END test9x4b.
