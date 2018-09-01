(* p.148 10t-11t *)

(* Procedure declarations *)

(* The procedure identifier is repeated at the end  *)
(*   of a procedure declaration.                    *)

(* P2 is wrong procedure identifier                 *)

MODULE test10a;
 PROCEDURE P1(): REAL;
  BEGIN
   RETURN 8.2;
  END P2;
 BEGIN
  ;
 END test10a.
