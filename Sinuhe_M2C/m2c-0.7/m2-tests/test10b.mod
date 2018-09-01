(* p.148 16t-17t *)

(* Procedure declarations *)

(* The function must contain RETURN with returned expression. *)

(* P1() has not RETURN                                        *)

MODULE test10b;
 PROCEDURE P1(): REAL;
  VAR
     x: REAL;
  BEGIN
   x:=2.7;
  END P1;
 VAR
    y: REAL;
 BEGIN
  y:=P1();
 END test10b.
