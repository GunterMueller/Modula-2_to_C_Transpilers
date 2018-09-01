MODULE PowersOf2;
 FROM InOut IMPORT Write,WriteLn,
		   WriteString,WriteInt;
 CONST M = 11; N = 32; (* M ~ N*log(2) *)
 VAR i,j,k,exp: INTEGER;
  c,r,t: INTEGER;
  d: ARRAY [0..M] OF INTEGER;
  f: ARRAY [0..N] OF INTEGER;

 BEGIN
  d[0] := 1; k := 1;
  FOR exp := 1 TO N DO
   (* compute d = 2^exp by d := 2*d *)
   c := 0; (*shift*)
   FOR i := 0 TO k-1 DO
    t := 2*d[i] + c;
    IF t >= 10 THEN
     d[i] := t - 10; c := 1
    ELSE
     d[i] := t; c := 0
    END
   END;
   IF c >0 THEN
    d[k] := 1; k := k + 1;
   END;
   (* output d[k-1]...d[0] *) i := M;
   REPEAT i := i - 1; Write(" "); UNTIL i = k;
   REPEAT i := i - 1; Write(CHR(d[i]+INTEGER(ORD("0"))))
   UNTIL i = 0;
   WriteInt(exp,4);
   (* compute f = 2^(-exp) by f := f DIV 2 and output it*)
   WriteString(" 0."); r := 0; (* remainder *)
   FOR j := 1 TO exp-1 DO
    r := 10*r + f[j]; f[j] := r DIV 2;
    r := r MOD 2; Write(CHR(f[j] + INTEGER(ORD("0"))))
   END;
   f[exp] := 5; Write("5"); WriteLn
  END
 END PowersOf2.
