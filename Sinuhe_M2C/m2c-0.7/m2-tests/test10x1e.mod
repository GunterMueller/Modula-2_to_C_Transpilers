(* p.149 17t-18t *)

(* Formal parameters *)

(* The formal array can be accessed elementwise only. *)

(* The assignment ActAr1:=FormAr is illegal           *)

MODULE test10x1e;
 TYPE
     ArType=ARRAY [2..10] OF REAL;
 VAR
    ActAr1,ActAr2: ArType;
		i: CARDINAL;
 PROCEDURE P(VAR FormAr: ARRAY OF REAL);
  BEGIN
   ActAr1:=FormAr;
  END P;
 BEGIN
  FOR i:=2 TO 10 DO
      ActAr2[i]:=2.8;
  END;
  P(ActAr2);
 END test10x1e.
