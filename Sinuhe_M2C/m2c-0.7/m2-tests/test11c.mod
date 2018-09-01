(* p.151 23b-22b *)

(* Modules *)

(* The unimported extern variable can not be used within the module. *)

(* The unimported variable  x is used within the module  M2          *)

MODULE test11c;
 MODULE M1;
  EXPORT x;
  VAR
     x: INTEGER;
  BEGIN
   x:=2;
  END M1;
 MODULE M2;
  VAR
     y: INTEGER;
  BEGIN
   y:=1;
   x:=y-1;
  END M2;
 BEGIN
  ;
 END test11c.
