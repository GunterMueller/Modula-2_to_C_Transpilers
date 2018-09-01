(* p.140 11t *)

(* Procedure types *)

(* The result type of the function procedure must be *)
(*   the same as the result type indicated in        *)
(*   the procedure type declaration.                 *)

(* The result type of procedure is different from one *)
(*   of the procedure type declaration                *)

 MODULE test6x8b;
  TYPE
      SubChar= ["a".."m"];
      ProcType= PROCEDURE(REAL): CHAR;
  VAR
     ProcVar: ProcType;
  PROCEDURE DifProc(x:REAL): SubChar;
   BEGIN
    IF x<2.24 THEN
     RETURN "b";
    ELSE
     RETURN "i";
    END;
   END DifProc;
  BEGIN
   ProcVar:= DifProc;
  END test6x8b.
