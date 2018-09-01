(* p. 137 14b *)

(* Enumerations *)

(* The identifiers constituting an enumeration type are used as *)
(*   constants inthe program.                                   *)

(* BBB is used as variable *)

 MODULE test6x2a;
  TYPE
      T=(AAA,BBB,CCC);
  VAR
      L:T;
  BEGIN
   L:=AAA;
   BBB:=L;
  END test6x2a.
