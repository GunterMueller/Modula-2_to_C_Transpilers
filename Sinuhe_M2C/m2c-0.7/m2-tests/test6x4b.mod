(* p.138 15t-16t *)

(* Array types *)

(* All array components must be of the same type.           *)

(* All array components are of the type CARDINAL therefore  *)
(*   last assignment is illegal.                            *)

 MODULE test6x4b;
  TYPE
      ArType=ARRAY [1..10] OF CARDINAL;
  VAR
     i:CARDINAL;
     ArVar: ArType;
  BEGIN
   FOR i:=1 TO 9 BY 2 DO
    ArVar[i]:=0;
    ArVar[i+1]:="C";
   END;
  END test6x4b.
