(* p.149 15t-16t *)

(* Formal parameters *)

(* The type of an open array parameter must be same     *)
(*   as the element type of the actual array.           *)

(* The type of Array is not CARDINAL                    *)

MODULE test10x1d;
 PROCEDURE P(VAR A: ARRAY OF CARDINAL);
  BEGIN
   A[0]:=0;
  END P;
 VAR
    Array: ARRAY [4..70] OF INTEGER;
 BEGIN
  P(Array);
 END test10x1d.
