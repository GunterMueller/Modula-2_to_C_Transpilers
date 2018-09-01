(* Compile with `-upper-case' flag. *)
IMPLEMENTATION MODULE Buffer;
 CONST N = 100;
 VAR in,out: [0..N-1];
   n: [0..N];
   buf: ARRAY[0..N-1] OF INTEGER;
 PROCEDURE Put(x: INTEGER);
 BEGIN
  IF n < N THEN
   buf[in] := x; in := (in+1) MOD N;
   n := n + 1; nonfull := n < N; nonempty := TRUE
  END
 END Put;
 PROCEDURE Get(VAR x: INTEGER);
 BEGIN
  IF n > 0 THEN
   x := buf[out]; out := (out+1) MOD N;
   n := n - 1; nonempty := n > 0; nonfull := TRUE
  END
 END Get;

BEGIN (*initiation*)
 n := 0; in := 0; out := 0;
 nonempty := FALSE; nonfull := TRUE
END Buffer.
