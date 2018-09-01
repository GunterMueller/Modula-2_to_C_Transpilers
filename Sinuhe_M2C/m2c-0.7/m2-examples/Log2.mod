MODULE Log2;
 FROM InOut IMPORT WriteString,WriteLn;
 FROM RealInOut IMPORT ReadReal,Done,WriteReal;
 VAR x,a,b,sum: REAL;

 BEGIN
  WriteString("x="); ReadReal(x);
  WHILE Done DO
   IF (x<1.0) OR (x>2.0) THEN
    WriteString("It is needed that 1.0 <= x < 2.0 .");
   ELSE
    (* 1.0 <= x <= 2.0 *)
    WriteReal(x,15);
    a := x; b := 1.0; sum := 0.0;
    REPEAT
     (* log2(x) = sum + b*log2(a) *)
     a := a*a; b := 0.5*b;
     IF a >= 2.0 THEN
      sum := sum + b; a := 0.5*a
     END
    UNTIL b < 1.0E-7;
    WriteString("  ");WriteReal(sum,16);
   END;
   WriteLn;
   WriteString("x="); ReadReal(x)
  END;
  WriteLn
END Log2.
