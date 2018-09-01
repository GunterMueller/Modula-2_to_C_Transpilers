MODULE Fractions;
 FROM InOut IMPORT Write,WriteLn,
		   WriteString,WriteInt;
 CONST Base = 10; N = 32;
 VAR i,j,m: INTEGER;
  rem: INTEGER;
  d: ARRAY[1..N] OF INTEGER; (*digits*)
  x: ARRAY[0..N] OF INTEGER; (* index *)

 BEGIN
  FOR i := 2 TO N DO
   FOR j := 0 TO i-1 DO x[j] := 0 END;
   m := 0; rem := 1;
   REPEAT m := m + 1; x[rem] := m;
    rem := Base * rem; d[m] := rem DIV i;
    rem := rem MOD i
   UNTIL x[rem] # 0;
   WriteInt(i,6); WriteString(" 0.");
   FOR j := 1 TO x[rem]-1 DO Write(CHR(d[j]+INTEGER(ORD("0")))) END;
   Write("'");
   FOR j := x[rem] TO m DO Write(CHR(d[j]+INTEGER(ORD("0")))) END;
   WriteLn
  END
 END Fractions.
