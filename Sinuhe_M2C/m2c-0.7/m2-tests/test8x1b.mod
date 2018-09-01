(* p.141 21t-22t *)

(* Operands *)

(* The actual var-parameter and corresponding formal *)
(*   one must be of the same type.                   *)

(* wrong actual parameter type *)

 MODULE test8x1b;
  VAR
     Card:CARDINAL;
      Res:REAL;
  PROCEDURE P(VAR Int:INTEGER):REAL;
   BEGIN
    IF Int#0 THEN RETURN 0.0;
    END;
    RETURN 1.0;
   END P;
  BEGIN
   Card:=0;
   IF P(Card)=0.0 THEN Res:=2.7;
   ELSE Res:=3.14;
   END;
  END test8x1b.
