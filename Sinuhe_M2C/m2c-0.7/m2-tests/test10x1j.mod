(* p.149 24t *)

(* Formal parameters *)

(* If a formal parameter specifies a procedure type then the actual one *)
(* can not be a standard procedure.                                     *)

(* CHR is a standard procedure                                          *)

MODULE test10x1j;
 TYPE
     ProcType=PROCEDURE(CARDINAL): CHAR;
 PROCEDURE P(x: ProcType): CHAR;
  BEGIN
   RETURN x(44);
  END P;
 VAR
    c:CHAR;
 BEGIN
  c:=P(CHR);
 END test10x1j.
