(* p.136 11t-13t *)

(* Declarations and scope rules *)

(* An identifier may be qualified. In this case it is prefixed   *)
(*   by another identifier which designates the module           *)
(*   (see Ch. 11) in which the qualified identifier is defined.  *)
(*   The prefix and the identifier are separated by a period.    *)

(* M3 is not a module name *)

 MODULE test4i;
  MODULE M1;
   EXPORT QUALIFIED y;
   VAR
      y: CHAR;
   BEGIN
    y:="A";
   END M1;

  MODULE M2;
   IMPORT M1;
   VAR y: CHAR;
   BEGIN
    y:=M3.y;
   END M2;

  BEGIN
   M1.y:="B";
  END test4i.
