(* p.151 23b-22b *)

(* Modules *)

(* The unexported variable can not be used outside the module. *)

(* x is not exported but it is used outside its module         *)

MODULE test11b;
 MODULE M;
  VAR
     x: INTEGER;
  BEGIN
   x:=2;
  END M;
 VAR
    y: INTEGER;
 BEGIN
  IF x=0 THEN y:=0;
  ELSE
     y:=1;
  END;
 END test11b.
