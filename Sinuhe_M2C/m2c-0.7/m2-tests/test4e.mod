(* p.136 3t-4t *)

(* Declarations and scope rules *)

(* A type T1 can be used in a declaration of pointer type  T     *)
(*   (see 6.7) which textually precedes the declaration of T1,   *)
(*   if both T and T1 are declared in the same block.            *)

(* types PX and X are defined in different blocks *)

 MODULE test4e;
  PROCEDURE FUNCT;
   TYPE
       PX= POINTER TO X;
   VAR
       VPX:PX;
   BEGIN
    VPX:=NIL;
   END FUNCT;
  TYPE X= ARRAY [1..20] OF REAL;
  BEGIN
   FUNCT;
  END test4e.
