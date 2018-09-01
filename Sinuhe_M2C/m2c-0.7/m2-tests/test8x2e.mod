(* p.142 20t-21t *)

(* Arithmetic operators *)

(* The division operator / applies to REAL operands only. *)

(* The wrong type pair for operation / *)

MODULE test8x2e;
 VAR
    RealVar,RealRes: REAL;
    IntVar: INTEGER;
 BEGIN
  RealVar:=24.0;
  IntVar:=-2;
  RealRes:=RealVar/IntVar;
 END test8x2e.
