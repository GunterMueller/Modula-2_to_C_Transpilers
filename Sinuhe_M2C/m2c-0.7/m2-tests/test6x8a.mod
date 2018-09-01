(* p.140 9t-11t *)

(* Procedure types *)

(* The types of the formal parameters of procedure must be  *)
(*   the same as those indicated in the formal type list    *)
(*   of the procedure type declaration.                     *)

(* The types of procedure parameters are different from ones *)
(*   of the procedure type declaration                       *)

 MODULE test6x8a;
  TYPE
      ProcType= PROCEDURE(VAR CHAR,VAR INTEGER);
  VAR
      ProcVar: ProcType;
  PROCEDURE DifProc(VAR c:CHAR; VAR p: CARDINAL);
   BEGIN
    IF p>2 THEN
     c:="h";
    ELSE
     c:="l";
    END;
   END DifProc;
  BEGIN
   ProcVar:=DifProc;
  END test6x8a.
