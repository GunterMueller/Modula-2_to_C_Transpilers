(* p.144 11b-10b *)

(* Procedure calls *)

(* For the formal value parameter the corresponding actual parameter *)
(* must be an expression.                                            *)

(* The first actual parameter of P2() is wrong                       *)

MODULE test9x2b;
 PROCEDURE P1(VAR x: CHAR);
  BEGIN
   x:="c";
  END P1;
 PROCEDURE P2(y: CARDINAL; VAR n: CARDINAL);
  BEGIN
   IF y=0 THEN
      n:=y;
   ELSE
      n:=y+2;
   END;
  END P2;
 VAR
    c: CHAR;
    M: CARDINAL;
 BEGIN
  c:="A";
  P2(P1(c),M);
 END test9x2b.
