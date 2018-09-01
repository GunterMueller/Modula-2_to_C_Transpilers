MODULE Harmonic;
 FROM InOut IMPORT ReadInt,Done,Write,WriteLn,WriteString;
 FROM RealInOut IMPORT WriteReal;
 VAR i,n: INTEGER;
  x,d,s1,s2: REAL;

 BEGIN
  WriteString("n="); ReadInt(n);
  WHILE Done DO
   s1 := 0.0; d := 0.0; i := 0;
   REPEAT
    d := d + 1.0; i := i + 1; s1 := s1 + 1.0/d;
   UNTIL i >= n;
   WriteReal(s1,24); s2:=0.0;
   REPEAT
    s2 := s2 + 1.0/d; d := d - 1.0; i := i - 1;
   UNTIL i = 0;
   WriteReal(s2,24); WriteLn;
   WriteString("n="); ReadInt(n);
  END;
  WriteLn
 END Harmonic.
