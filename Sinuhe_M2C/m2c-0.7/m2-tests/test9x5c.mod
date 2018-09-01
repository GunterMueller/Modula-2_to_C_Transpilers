(* p.145 16b-15b *)

(* Case statements *)

(* Case labels are constants.*)

(* y is not constant         *)

MODULE test9x5c;
 VAR
    x,y: INTEGER;
    c: CHAR;
 BEGIN
  x:=2;
  y:=3;
  CASE x OF
   1: ; |
   y: c:="A"
  END;
 END test9x5c.
