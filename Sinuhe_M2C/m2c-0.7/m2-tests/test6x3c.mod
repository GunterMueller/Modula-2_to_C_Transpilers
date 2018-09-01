(* p.137 1b *)

(* Subrange types *)

(* The bounds of a subrange type must be a constants *)

(* the bounds of the subrange type WRONG are not constants *)

MODULE test6x3c;
  MODULE M;
   EXPORT VX;
   VAR VX:CARDINAL;
   BEGIN
    VX:=33;
   END M;
 CONST C=22;
 TYPE WRONG=[14..C+VX];
 VAR N:CARDINAL;
 BEGIN
  N:=18;
 END test6x3c.
