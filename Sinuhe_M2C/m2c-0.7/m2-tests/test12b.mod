(* p.153 10b-8b *)

(* System-dependent facilities *)

(* If a formal parameter is of type WORD, the actual parameter  *)
(*   may be of any type that uses one storage word.             *)

(* The actual parameter is too long for WORD                    *)

MODULE test12b;
 FROM SYSTEM IMPORT WORD;
 PROCEDURE P(VAR x: WORD);
  VAR y: WORD;
  BEGIN
   y:=x;
   x:=y;
  END P;
 VAR
    M: ARRAY [0..40] OF REAL;
 BEGIN
  P(M);
 END test12b.
