(* p.140 12t-13t *)

(* Procedure types *)

(* The value of procedure type must not be a standard  *)
(*   procedure.                                        *)

(* CHR is the standard procedure *)

 MODULE test6x8d;
  TYPE
      ProcType= PROCEDURE(CARDINAL): CHAR;
  VAR ProcVar: ProcType;
  BEGIN
   ProcVar:=CHR;
  END test6x8d.
