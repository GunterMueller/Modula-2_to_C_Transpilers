(* p.150 23t-25t *)

(* Standard procedures *)

(* In ORD(x) x is of any enumeration types, CHAR, INTEGER, or CARDINAL. *)

(* x is of subrange type                                                *)

MODULE test10x2b;
 TYPE
     SubrangeType=[24..36];
 VAR
    x: SubrangeType;
    c: CARDINAL;
 BEGIN
  x:=30;
  c:=ORD(x);
 END test10x2b.
