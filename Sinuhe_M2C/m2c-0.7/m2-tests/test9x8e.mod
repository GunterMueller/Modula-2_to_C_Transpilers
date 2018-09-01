(* p.146 13b-12b *)

(* For statements *)

(* The control variable should not be changed by the statement sequence. *)

(* X is changed by the statement sequence                                *)

MODULE test9x8e;
 PROCEDURE Pr(VAR x: INTEGER);
  BEGIN
   IF x>4 THEN
      x:=0;
   ELSE
      x:=4;
   END;
  END Pr;
 VAR
    X: INTEGER;
    P: REAL;
 BEGIN
  P:=1.0;
  FOR X:=0 TO 20 DO
      P:=P*4.0;
      Pr(X);
  END;
 END test9x8e.
