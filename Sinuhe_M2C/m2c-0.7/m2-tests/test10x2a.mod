(* p.150 18t *)

(* Standard procedures *)

(* in FLOAT(x) x must be of INTEGER type. *)

(* c is of type CARDINAL                  *)

MODULE test10x2a;
 VAR
    r: REAL;
    c: CARDINAL;
 BEGIN
  c:=3;
  r:=FLOAT(c);
 END test10x2a.
