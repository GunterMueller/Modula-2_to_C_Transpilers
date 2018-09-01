(* p.135 10b-9b *)

(* Declarations and scope rules *)

(* Every identifier occuring in a program must be introduced *)
(*   by a declaration, unless it is a standard identifier.   *)

(* B was not declared *)

 MODULE test4a;
  VAR
     A:CARDINAL;
  BEGIN
   A:=3+B;
  END test4a.
