(* p.148 3b-2b *)

(* Formal parameters *)

(* Variable parameters correspond to actual parameters *)
(*   that are variables.                               *)

(* For formal variable parameter the actual one is an  *)
(*   expression                                        *)

MODULE test10x1a;
 PROCEDURE P1(VAR x: REAL);
  BEGIN
   x:=x-2.4;
  END P1;
 VAR
    t: REAL;
 BEGIN
  t:=0.2;
  P1(t+2.2);
 END test10x1a.
