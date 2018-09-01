(* p.153 3b *)

(* System-dependent facilities *)

(* The type ADDRESS is compatible with all pointer types,  *)
(*   and also with the type CARDINAL.                      *)

(* The type ADDRESS is not compatible with CHAR            *)

MODULE test12c;
 FROM SYSTEM IMPORT ADDRESS;
 VAR
    c: CHAR;
    a: ADDRESS;
 BEGIN
  c:="A";
  a:=c;
 END test12c.
