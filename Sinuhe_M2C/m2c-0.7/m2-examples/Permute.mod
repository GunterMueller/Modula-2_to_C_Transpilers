MODULE Permute;
 FROM InOut IMPORT Read,Write,WriteLn;

 VAR n: INTEGER; ch: CHAR;
  a: ARRAY[1..20] OF CHAR;

 PROCEDURE Output;
  VAR i: INTEGER;
 BEGIN
  FOR i := 1 TO n DO Write(a[i]) END;
  WriteLn
 END Output;

 PROCEDURE Permute(k: INTEGER);
  VAR i: INTEGER; t: CHAR;
 BEGIN
  IF k = 1 THEN Output
  ELSE Permute(k-1);
   FOR i := 1 TO k-1 DO
    t := a[i]; a[i] := a[k]; a[k] := t;
    Permute(k-1);
    t := a[i]; a[i] := a[k]; a[k] := t;
   END
  END
 END Permute;

BEGIN
 Write(">"); n := 0; Read(ch);
 WHILE ch > " " DO
  n := n + 1; a[n] := ch; Read(ch)
 END;
 WriteLn; Permute(n);
END Permute.
