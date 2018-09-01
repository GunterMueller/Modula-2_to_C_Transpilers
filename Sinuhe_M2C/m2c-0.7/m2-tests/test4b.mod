(* p.135 10b-9b *)

(* Declarations and scope rules *)

(* Every identifier occuring in a program must be introduced *)
(*   by a declaration, unless it is a standard identifier.   *)

(* The standard identifier NIL is declared *)

 MODULE test4b;
  TYPE
      NIL=CARDINAL;
  VAR
      A:NIL;
  BEGIN
   A:=24;
  END test4b.
