(* p.151 12t-13t *)

(* Modules *)

(* The module identifier is repeated at the end of the declaration. *)

(* The end of M is wrong                                            *)

MODULE test11a;
 MODULE M;
  VAR
     a: CHAR;
  BEGIN
   a:="k";
  END;
 BEGIN
  ;
 END test11a.
