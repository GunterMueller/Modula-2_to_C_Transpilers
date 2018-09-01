(* p.150 12b-10b *)

(* Standard types *)

(* In VAL(T,x) T is any enumeration type, or CHAR, INTEGER, or CARDINAL. *)

(* T is a subrange type                                                  *)

MODULE test10x2d;
 TYPE
     T= [20..70];
 VAR
    x: T;
    y: CARDINAL;
 BEGIN
  y:=30;
  x:=VAL(T,y);
 END test10x2d.
