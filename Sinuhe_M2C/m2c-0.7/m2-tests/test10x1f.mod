(* p.149 18t-19t *)

(* Formal parameters *)

(* The formal array may occur as actual parameter whose formal one is *)
(* without specified index bounds.                                    *)

(* The call P1(FormAr) is illegal                                     *)

MODULE test10x1f;
 TYPE
     ArType= ARRAY [2..10] OF REAL;
 VAR
    ActAr1,ActAr2: ArType;
 PROCEDURE P(VAR FormAr: ARRAY OF REAL);
  PROCEDURE P1(VAR FormAr1: ArType);
   BEGIN
    FormAr1[2]:=ActAr2[10];
   END P1;
  BEGIN
   P1(FormAr);
  END P;
  BEGIN
   ActAr2[10]:=2.7;
   P(ActAr1);
  END test10x1f.
