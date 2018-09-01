(* p.153 11b-10b *)

(* System-dependent facilities *)

(* No operation except assignment is defined on the type WORD. *)

(* The comparison of two words is illegal                      *)

MODULE test12a;
 FROM SYSTEM IMPORT WORD;
 VAR
    x,y: CARDINAL;
    r: BOOLEAN;
 PROCEDURE P(W1,W2:WORD);
  BEGIN
   r:=((W1+W2)=1);
  END P;
 BEGIN
  x:=1;
  y:=2;
  P(x,y);
 END test12a.
