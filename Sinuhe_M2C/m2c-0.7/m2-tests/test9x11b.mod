(* p.147 2b-1b *)

(* Return statement *)

(* The function must contain a RETURN operator returning a value. *)

(* I() returns nothing                                            *)

MODULE test9x11b;
 PROCEDURE I(): INTEGER;
  VAR
     x: INTEGER;
  BEGIN
   x:=2;
   RETURN;
  END I;
 VAR
    k: INTEGER;
 BEGIN
  k:=I();
 END test9x11b.
