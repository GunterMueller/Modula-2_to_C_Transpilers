(* p.144 8b *)

(* Procedure calls *)

(* The types of corresponding actual and formal parameters must *)
(* be compatible in the case of variable parameters.            *)
(* Section 10.1 says that these types must be identical !!!     *)

(* I and Card are not compatible                                *)

MODULE test9x2c;
 PROCEDURE P(VAR I: INTEGER; VAR C: CARDINAL);
  BEGIN
   C:=6+I;
  END P;
 VAR
    Card,Card1: CARDINAL;
 BEGIN
  Card:=7;
  P(Card,Card1);
 END test9x2c.
