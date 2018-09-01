(* p.144 7b *)

(* Procedure calls *)

(* The formal parameter  and corresponding actual one must be    *)
(*   assignment compatible in the case of value parameters.      *)

(* The formal parameter R is not assignment compatible with      *)
(*   the corresponding actual one                                *)

MODULE test9x2d;
 PROCEDURE P(VAR I: INTEGER; R: REAL);
  BEGIN
   IF R<2.0 THEN I:=0;
   ELSE
      I:=1;
   END;
  END P;
 VAR
    Int: INTEGER;
    R: REAL;
 BEGIN
  R:=4.8;
  P(Int,TRUNC(R));
 END test9x2d.
