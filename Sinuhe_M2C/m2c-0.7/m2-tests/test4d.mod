(* p.136 1t-2t *)

(* Declarations and scope rules *)

(* If an identifier x defined by a declaration D1 is used in   *)
(*   another declaration ( not statement ) D2, then D1 must    *)
(*   textually precede D2.                                     *)

(* B was defined before A *)

 MODULE test4d;
  CONST
       B=A+3;
       C=20;
       A=0;
  VAR D:CARDINAL;
  BEGIN
   D:=(A+B)*C;
  END test4d.
