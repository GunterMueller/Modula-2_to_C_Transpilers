(* p.138 15t *)

(* Array types *)

(* An array is a structure consisting of a fixed number    *)
(*   of components.                                        *)

(* The using of nonexistent component of array ArVar *)

 MODULE test6x4a;
  CONST
       RealConst= 128.44;
  VAR
      i: CARDINAL;
  TYPE
      IndType= [1..20];
      ArType = ARRAY IndType OF REAL;
  VAR
      ArVar : ArType;
      RealVar: REAL;
  BEGIN
   i:=10;
   RealVar:=3.14;
   ArVar[3*i]:=RealVar;
  END test6x4a.
