(* p.145 15b *)

(* Case statements *)

(* Every value must occur only once as label.*)

(* 7,8,9 are common for both cases           *)

MODULE test9x5d;
 VAR
    x: INTEGER;
    c: CHAR;
 BEGIN
  x:=2;
  CASE x OF
   1,2,7..18: c:="A" |
      4,5..9: c:="B"
  END;
 END test9x5d.
