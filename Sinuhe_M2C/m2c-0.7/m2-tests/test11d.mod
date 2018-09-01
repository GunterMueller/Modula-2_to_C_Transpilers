(* p.151 22b-20b *)

(* Modules *)

(* an unqualified using of qualified export is illegal. *)

(* In M2 x must be qualified                            *)

MODULE test11d;
 MODULE M1;
  EXPORT QUALIFIED x;
  VAR
     x: INTEGER;
  BEGIN
   x:=2;
  END M1;
 MODULE M2;
  IMPORT x;
  BEGIN
   DEC(x);
  END M2;
 BEGIN
  ;
 END test11d.
