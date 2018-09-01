(* p.146 14b-13b *)

(* For statements *)

(* The control variable can not be a component of structured variable. *)

(* i is a structured variable                                          *)

MODULE test9x8a;
 VAR
    i: RECORD
	k: INTEGER;
	p: REAL
       END;
    BEGIN
     i.p:=2.4;
     FOR i.k:=1 TO 10 DO
	 i.p:=i.p*4.2+3.3;
     END;
    END test9x8a.
