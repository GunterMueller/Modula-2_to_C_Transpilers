MODULE gcd;
FROM InOut IMPORT ReadInt,WriteString,WriteLn,WriteInt;
VAR x,y:INTEGER;

BEGIN
 WriteString('x=');ReadInt(x);WriteLn;
 WriteString('y=');ReadInt(y);WriteLn;
 WHILE x#y DO
  IF x>y THEN x:=x-y
       ELSE y:=y-x
  END
 END;
 WriteString('gcd=');WriteInt(x,6);WriteLn;
END gcd.
