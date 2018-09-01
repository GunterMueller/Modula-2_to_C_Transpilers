(* p.155 2t *)

(* Creating a process and transfer of control *)

(* The process body must be a parameterless procedure. *)

(* procedure PR has a parameter                        *)

MODULE test13x1a;
 FROM SYSTEM IMPORT NEWPROCESS,ADDRESS;
 PROCEDURE PR(x: INTEGER);
  BEGIN
   DEC(x);
  END PR;
 PROCEDURE PR1(A: ADDRESS; n: CARDINAL; p1: ADDRESS);
  BEGIN
   NEWPROCESS(PR,A,n,p1);
  END PR1;
 BEGIN
  ;
 END test13x1a.
