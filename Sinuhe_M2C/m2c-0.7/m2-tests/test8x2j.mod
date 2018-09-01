(* p.143 17b-19b *)

(* Relations *)

(* In an expression of the form  x IN s  ,the expression  *)
(*   s must be of type  SET OF T  ,where T is compatible  *)
(*   with the type of x.                                  *)

(* The wrong (not compatible) operand pair in IN relation *)

MODULE test8x2j;
 VAR
    s1: BITSET;
    x: REAL;
    t: BOOLEAN;
 BEGIN
  s1:={0,1,2,8};
  x:=1.0;
  t:= x IN s1 ;
 END test8x2j.
