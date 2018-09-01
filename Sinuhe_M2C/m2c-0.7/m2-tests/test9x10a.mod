(* p.147 14b *)

(* With statements *)

(* The WITH statement opens a new scope *)

(* The field x is not visible outside WITH *)

MODULE test9x10a;
 VAR
    r: RECORD
	x,y: REAL
       END;
 BEGIN
  WITH r DO
   y:=2.8;
  END;
  x:=3.2;
 END test9x10a.
