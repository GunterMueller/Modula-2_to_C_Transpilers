(* p.147 5b-6b *)

(* Return statements *)

(* The type of a return expression must be assignment compatible with *)
(*   the type of  function result.                                    *)

(* 2 is not of type REAL                                              *)

MODULE test9x11a;
 PROCEDURE P(): REAL;
  BEGIN
   RETURN 2;
  END P;
 VAR
    x: REAL;
 BEGIN
  x:=P();
 END test9x11a.
