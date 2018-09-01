MODULE sieve;
FROM InOut IMPORT Write,WriteInt;
 CONST
  SieveSize=8190;
 VAR
  i,prime,k,count,iter:INTEGER;
  flags:ARRAY[0..SieveSize]OF BOOLEAN;

 BEGIN
  Write(CHR(7));
  FOR iter:=1 TO 10 DO
   count:=0;
   FOR i:=0 TO SieveSize DO
    flags[i]:=TRUE;
   END;
   FOR i:=0 TO SieveSize DO
    IF flags[i] THEN
     prime:=i+i+3;
     k:=i+prime;
     WHILE (k<=SieveSize) DO
      flags[k]:=FALSE;
      k:=k+prime;
     END;
     INC(count);
    END;
   END;
  END;
  Write(CHR(7));
  WriteInt(count,1);
 END sieve.
