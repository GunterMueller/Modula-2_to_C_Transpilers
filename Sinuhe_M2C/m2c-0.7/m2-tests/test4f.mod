(* p.136 6t-7t *)

(* Declarations and scope rules *)

(* If an identifier defined in a module M1 is exported,          *)
(*   the scope expands over the block which contains M1,         *)
(*   it extends to all those units which import M1.              *)

(* x is not wisible in main module *)

MODULE test4f;
 MODULE LEVEL1;
  MODULE LEVEL2;
   EXPORT x;
   VAR
      x:REAL;
   BEGIN
    x:=1.12;
   END LEVEL2;
  VAR
     y:REAL;
  BEGIN
   y:=2.2;
   x:=0.7;
  END LEVEL1;
 BEGIN
  x:=8.8; (* x is not wisible here *)
 END test4f.
