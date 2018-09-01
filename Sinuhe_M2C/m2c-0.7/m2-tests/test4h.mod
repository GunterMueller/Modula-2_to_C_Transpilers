(* p.136 9t-10t *)

(* Declarations and scope rules *)

(* Field identifiers of a record declaration (see 6.5) are valid *)
(*   only in field designators and in with statements referring  *)
(*   to a variable of that record type.                          *)

(* xc is not allowed in z *)

 MODULE test4h;
  TYPE
      xtype= RECORD
	      xc:CHAR;
	     END;

      ztype= RECORD
	      zc:CHAR;
	     END;
  VAR
     x: xtype;
     z: ztype;
     y: CHAR;
  BEGIN
   WITH z DO
    zc:="A";
    xc:=zc;
   END;
  END test4h.
