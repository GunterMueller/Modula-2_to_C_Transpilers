(* p.155 7t-8t *)

(* Creating a process and transfer of control *)

(* Evidently, p2 must have been assigned a process by an  *)
(*   earlier call to either NEWPROCESS or TRANSFER.       *)

(* p2 is not a process address                            *)

MODULE test13x1c;
 FROM SYSTEM IMPORT NEWPROCESS,TRANSFER,ADDRESS;
 IMPORT Storage;
 PROCEDURE Process;
  VAR
     x: CHAR;
     p1,p2: ADDRESS;
  BEGIN
   x:="A";
   TRANSFER(p1,p2);
  END Process;
 VAR
    A,p: ADDRESS;
 BEGIN
  IF Storage.Available(400) THEN
     Storage.ALLOCATE(A,400);
     NEWPROCESS(Process,A,400,p);
  END;
 END test13x1c.
