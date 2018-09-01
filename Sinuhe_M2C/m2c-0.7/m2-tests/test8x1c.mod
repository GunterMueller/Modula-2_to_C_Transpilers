(* p.141 21t-22t *)

(* Operands *)

(* The types of actual and corresponding formal  *)
(*   parameters must be assignment compatible    *)
(*   in the case of value parameters.            *)

(* wrong actual parameter type *)

 MODULE test8x1c;
  VAR
     Cr:CHAR;
      Res:REAL;
  PROCEDURE P(Int:INTEGER):REAL;
   BEGIN
    IF Int#0 THEN RETURN 0.0;
    END;
    RETURN 1.0;
   END P;
  BEGIN
   Cr:="A";
   IF P(Cr)=0.0 THEN Res:=2.7;
   ELSE Res:=3.14;
   END;
  END test8x1c.
