(* p.136 9t-10t *)

(* Declarations and scope rules *)

(* Field identifiers of a record declaration (see 6.5) are valid *)
(*   only in field designators and in with statements reffering  *)
(*   to a variable of that record type.                          *)

(* xc is not used as field name *)

 MODULE test4g;
  VAR
     x:RECORD
	xc:CHAR;
       END;
     y:CHAR;
  BEGIN
   x.xc:="A";
   y:=xc;
  END test4g.
