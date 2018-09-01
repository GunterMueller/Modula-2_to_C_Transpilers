(* p.137 4b-3b *)

(* Subrange types *)

(* A type T may be defined as a subrange of another,       *)
(*   basic or enumeration type T1 ( except REAL ).         *)

(* REAL subrange *)

 MODULE test6x3a;
  TYPE T= REAL[1.24..3.58];
  VAR V: REAL;
  BEGIN
   V:=9.82;
  END test6x3a.
