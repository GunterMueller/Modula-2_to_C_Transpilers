(* p.148 4t-6t *)

(* Exit statements *)

(* An EXIT statement specifies termination of the *)
(*   enclosing LOOP statement.                    *)

(* EXIT without LOOP.                             *)

MODULE test9x11c;
 VAR
    c: CHAR;
 BEGIN
  c:="A";
  EXIT;
 END test9x11c.
