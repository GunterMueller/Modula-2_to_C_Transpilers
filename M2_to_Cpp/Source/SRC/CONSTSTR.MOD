IMPLEMENTATION MODULE ConstStr;

  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT String, Addr; (* BMB *)

  PROCEDURE Init(VAR cs: T; s: ARRAY OF CHAR);
    VAR
      i: CARDINAL;
      p: PointerToChar;
  BEGIN
    cs.len := String.Length(s);
    ALLOCATE(cs.p, SIZE(CHAR)*(cs.len+1));
    p := cs.p;
    FOR i := 1 TO cs.len DO
      p^ := s[i-1];
      Addr.Inc(p, LONGCARD(SIZE(CHAR)));
    END;
    p^ := 0C;
  END Init;

  PROCEDURE ToStr(VAR s: ARRAY OF CHAR; cs: T);
    VAR
      i: CARDINAL;
      p: PointerToChar;
  BEGIN
    i := 0;
    p := cs.p;
    WHILE (i <= HIGH(s)) AND (i <= cs.len) DO
      s[i] := p^;            (* ^^^^^^^^^ will copy 0C at the end *)
      Addr.Inc(p, LONGCARD(SIZE(CHAR)));
      INC(i);
    END;
  END ToStr;

  PROCEDURE Kill(VAR cs: T);
  BEGIN
    DEALLOCATE(cs.p, SIZE(CHAR)*(cs.len+1));
    cs.p := NIL;
    cs.len := 0;
  END Kill;

END ConstStr.
