(* p.149 10t-11t *)

(* Formal parameters *)

(* The formal and actual value parameters must be assignment compatible. *)

(*The first actual parameter in P1() is of type CARDINAL                 *)

MODULE test10x1c;
 PROCEDURE P1(x: REAL; VAR n: CARDINAL);
  BEGIN
   IF x#2.0 THEN n:=1;
   ELSE
      n:=0;
   END;
  END P1;
 VAR
    N: CARDINAL;
 BEGIN
  P1(24,N);
 END test10x1c.
