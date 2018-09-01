(* p.142 22t-23t *)

(* Arithmetic operators *)

(* Sign inversion applies to operands of type    *)
(*   INTEGER or REAL.                            *)

(* a sign inversion is not applicable to CARDINAL *)

MODULE test8x2f;
 VAR
    x: CARDINAL;
 BEGIN
  x:=2;
  IF (-x)>10 THEN
     x:=1;
  ELSE
     x:=0;
  END;
 END test8x2f.
