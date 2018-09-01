MODULE Primes;
 FROM InOut IMPORT WriteLn, WriteInt;
 CONST N = 250; M = 16; (* M^2 ~ N *)
  LL = 8; (* simple numbers on a line *)

 VAR i,k,x: INTEGER;
  step,bound,square,L: INTEGER;
  simple: BOOLEAN;
  P,V: ARRAY[0..M] OF INTEGER;

 BEGIN
  L := 0;
  x := 1; step := 4; bound := 1; square := 9;
  FOR i := 3 TO N DO
   (* find next simple number P[i] *)
   REPEAT x := x + step; step := 6 - step;
    IF square <= x THEN
     bound := bound + 1; V[bound] := square;
     square := P[bound+1]*P[bound+1]
    END;
    k := 2; simple := TRUE;
    WHILE simple & (k < bound) DO
     k := k + 1;
     IF V[k] < x THEN
      V[k] := V[k] +2*P[k]
     END;
     simple := x # V[k]
    END
   UNTIL simple;
   IF i<= M THEN P[i] := x END;
   WriteInt(x,6); L := L + 1;
   IF L = LL THEN
    WriteLn; L := 0
   END
  END
 END Primes.
