(* p.140 12t *)

(* Procedure types *)

(* The procedure that is value of a procedure type     *)
(*   must not be declared local to another procedure.  *)

(* P2 must not be declared local to procedure P1 *)

 MODULE test6x8c;
  TYPE
      ProcType= PROCEDURE(REAL): REAL;
  VAR
      ProcVar: ProcType;
	    t: REAL;
  PROCEDURE P1;
   PROCEDURE P2(x:REAL): REAL;
    BEGIN
     RETURN (x+2.4);
    END P2;
   BEGIN
    ProcVar:=P2;
   END P1;
  BEGIN
   P1;
   t:=ProcVar(4.4);
  END test6x8c.
