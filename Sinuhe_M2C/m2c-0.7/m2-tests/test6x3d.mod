(* p.138 2t- 4t *)

(* Subrange types *)

(* A value to be assigned to a variable of a subrange type *)
(*   must lie within the specified interval.               *)

(* X is not inside subrange *)

 MODULE test6x3d;
  TYPE
      T= [1..3];
  VAR
      V: CARDINAL;
      X:T;
  BEGIN
   V:=9;
   X:=V-1;
  END test6x3d.
