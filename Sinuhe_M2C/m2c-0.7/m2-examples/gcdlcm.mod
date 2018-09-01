MODULE gcdlcm;
FROM InOut IMPORT ReadInt,WriteLn,WriteString,WriteInt;
VAR x,y,u,v:INTEGER;

BEGIN
 WriteString("x=");ReadInt(x);WriteLn;
 WriteString("y=");ReadInt(y);
 u:=x;v:=y;
 WHILE x#y DO
  (* mcd(x,y) = mcd(x0,y0), x*v + y*u = 2*x0*y0 *)
  IF x > y THEN
   x := x - y; u := u + v;
  ELSE
   y := y - x; v := v + u;
  END
 END;
 WriteInt(x,6); WriteInt((u +v) DIV 2,6); WriteLn;
END gcdlcm.
