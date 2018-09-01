(* p.141. 10t-11t *)

(* Operands *)

(* For array the index type must be assignment compatible *)
(*   with the index expression type.                      *)

(* index type T1 is not compatible with expression type T2 *)

MODULE test8x1a;
 TYPE
     T1=(a,b,c,d);
     T2=(i,j,k,l);
     ArType=ARRAY T1 OF REAL;
 VAR
     ArVar:ArType;
     V:T2;
 BEGIN
  V:=k;
  ArVar[V]:=2.11;
 END test8x1a.
