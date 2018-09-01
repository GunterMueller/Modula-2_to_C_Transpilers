(* p.145 16b *)

(* Case statements *)

(* All labels must be compatible with the type of the case expression .*)

(* -1 is not compatible with CARDINAL                                  *)

MODULE test9x5b;
 VAR
    x: CARDINAL;
    y: CHAR;
 BEGIN
  x:=2;
  CASE x OF
   -1:  y:="A"|
    4:  y:="B";
   ELSE y:="C";
  END;
 END test9x5b.
