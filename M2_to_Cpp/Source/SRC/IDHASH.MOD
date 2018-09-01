IMPLEMENTATION MODULE IdHash;

  FROM Defs IMPORT IdStr, PIdStr;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT String; (* BMB *)

  PROCEDURE Hash(VAR s: ARRAY OF CHAR): CARDINAL;
    VAR
      i, sum: CARDINAL;
  BEGIN
    sum := 0;
    i := 0;
    LOOP
      IF i > HIGH(s) THEN EXIT END;
      IF s[i] = 0C   THEN EXIT END;
      sum := (sum + ORD(s[i])) MOD PrimeNum;
      INC(i);
    END;
    RETURN sum;
  END Hash;

  PROCEDURE Find(VAR s: IdStr;
                 VAR t: T;
                 VAR index: CARDINAL;
                 VAR old: List;
                 VAR found: PIdStr);
    VAR
      curr: List;
  BEGIN
    index := Hash(s);
    curr := t[index];
    old := NIL;
    LOOP
      IF curr = NIL THEN EXIT END;
      IF String.Equal(curr^.p^, s) THEN EXIT END;
      old := curr;
      curr := curr^.next;
    END;
    IF curr = NIL THEN
      found := NIL;
    ELSE
      found := curr^.p;
    END;
  END Find;

  PROCEDURE Init(VAR t: T);
    VAR
      i: CARDINAL;
  BEGIN
    FOR i := 0 TO MaxArray DO
      t[i] := NIL;
    END;
  END Init;

  PROCEDURE IsMember(VAR t: T; s: IdStr): BOOLEAN;
    VAR
      index: CARDINAL;
      old: List;
      found: PIdStr;
  BEGIN
    Find(s, t, index, old, found);
    RETURN found # NIL;
  END IsMember;

  PROCEDURE Insert(VAR t: T; s: IdStr; VAR p: PIdStr);
    VAR
      index: CARDINAL;
      old, tmp: List;
  BEGIN
    Find(s, t, index, old, p);
    IF p = NIL THEN
      NEW(tmp);
      NEW(tmp^.p);
      String.Copy(tmp^.p^, s);
      IF old = NIL THEN
        tmp^.next := NIL;
        t[index] := tmp;
      ELSE
        tmp^.next := old^.next;
        old^.next := tmp;
      END;
      p := tmp^.p;
    END;
  END Insert;

  PROCEDURE Remove(VAR t: T; s: IdStr);
    VAR
      index: CARDINAL;
      old, tmp: List;
      found: PIdStr;
  BEGIN
    Find(s, t, index, old, found);
    IF found # NIL THEN
      IF old = NIL THEN
        tmp := t[index];
        t[index] := t[index]^.next;
      ELSE
        tmp := old^.next;
        old^.next := old^.next^.next;
      END;
      DISPOSE(tmp^.p);
      DISPOSE(tmp);
    END;
  END Remove;

  PROCEDURE Kill(VAR t: T);
    VAR
      i: CARDINAL;
      old, tmp: List;
  BEGIN
    FOR i := 0 TO MaxArray DO
      old := t[i];
      LOOP
        IF old = NIL THEN EXIT END;
        tmp := old^.next;
        DISPOSE(old^.p);
        DISPOSE(old);
        old := tmp;
      END;
    END;
  END Kill;

END IdHash.
