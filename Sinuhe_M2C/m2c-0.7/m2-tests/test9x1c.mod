(* p.144 12t-13t *)

(* Assignments *)

(* A string of length n1 can be assigned to an array variable  *)
(*   with n2>n1 elements of type CHAR.                         *)

(* the string assignment for n2=n1                             *)

MODULE test9x1c;
 CONST STR="123456";
 VAR
    Ar: ARRAY [26..31] OF CHAR;
 BEGIN
  Ar:=STR;
 END test9x1c.
