(* p.149 22t-24t *)

(* Formal parameters *)

(* If a formal parameter specifies a procedure type then the corresponding *)
(* actual parameter must not be a procedure which level is not zero.       *)

(* The level of R is not zero                                              *)

MODULE test10x1i;
 TYPE
     ProcType=PROCEDURE(VAR  REAL);
 PROCEDURE P;
  PROCEDURE R(VAR x: REAL);
   BEGIN
    x:=x*4.0;
   END R;
  PROCEDURE S(y:ProcType): REAL;
   VAR
      t: REAL;
   BEGIN
    t:=1.0;
    y(t);
    RETURN t;
   END S;
  VAR
     u: REAL;
  BEGIN
   u:=S(R);
  END P;
 BEGIN
  ;
 END test10x1i.
