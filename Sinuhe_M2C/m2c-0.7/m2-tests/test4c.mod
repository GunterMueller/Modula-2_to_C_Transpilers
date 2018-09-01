(* p.135 3b-2b *)

(* declarations and scope rules *)

(* The scope extends over the entire block ( procedure or        *)
(*   module declaration) to which the declaration belongs and    *)
(*   to which the object is local.                               *)

(* LockVar is not visible outside P1 *)

MODULE test4c;

 PROCEDURE P1():CARDINAL;
  VAR
     LockVar:CARDINAL;
  BEGIN
   LockVar:=1; GlobVar:=0;
   RETURN LockVar+3;
  END P1;

 VAR
    GlobVar:CARDINAL;
 BEGIN
  GlobVar:=3*LockVar+P1();
 END test4c.
