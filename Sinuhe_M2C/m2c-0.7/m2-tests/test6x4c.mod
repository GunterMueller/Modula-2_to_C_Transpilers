(* p.138 18t-19t *)

(* Array types *)

(* The index type must be an enumeration, a subrange type,   *)
(*   or one of the basic types BOOLEAN or CHAR.              *)

(* The type IndType of array index is wrong *)

 MODULE test6x4c;
  TYPE
      IndType= ARRAY [1..12] OF CARDINAL;
      ArType = ARRAY IndType OF REAL;
  VAR
      ArVar: ArType;
	  b: CARDINAL;
  BEGIN
   b:=0;
  END test6x4c.
