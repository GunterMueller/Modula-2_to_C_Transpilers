(* p.155 2t-3t *)

(* Creating a process and transfer of control *)

(* The process is a procedure declared at level 0. *)

(* The level of procedure  process  is not 0       *)

MODULE test13x1b;
 FROM SYSTEM IMPORT NEWPROCESS,ADDRESS;
 IMPORT Storage;
 PROCEDURE P(
      A: ADDRESS;
      n: CARDINAL;
     p1: ADDRESS);
   PROCEDURE process;
    VAR
       x: CHAR;
    BEGIN
     x:="A";
    END process;
   PROCEDURE PR;
    BEGIN
     NEWPROCESS(process,A,n,p1);
    END PR;
  BEGIN
   PR;
  END P;
 BEGIN
  ;
 END test13x1b.
