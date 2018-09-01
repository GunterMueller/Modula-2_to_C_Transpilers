IMPLEMENTATION MODULE Ids;

  FROM Defs IMPORT IdStr;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT String;

  PROCEDURE Init(VAR l: List);
  BEGIN
    l.first := NIL;
    l.last := NIL;
    l.curr := NIL;
  END Init;

  PROCEDURE Insert(VAR l: List; name: IdStr);
    VAR
      tmp: IdPtr;
  BEGIN
    NEW(tmp);
    String.Copy(tmp^.name, name);
    tmp^.next := NIL;
    IF l.first = NIL THEN
      l.first := tmp;
    ELSE
      l.last^.next := tmp;
    END;
    l.last := tmp;
  END Insert;

  PROCEDURE Replace(VAR l: List; name: IdStr; ins: List);
    VAR
      tmp, tmp2, insPtr: IdPtr;
      found: BOOLEAN;
  BEGIN
    insPtr := ins.first;
    IF insPtr # NIL THEN
      tmp := l.first;
      LOOP
        IF tmp = NIL THEN EXIT END;
        IF String.Equal(tmp^.name, name) THEN
          String.Copy(tmp^.name, insPtr^.name);
          insPtr := insPtr^.next;
          WHILE insPtr # NIL DO
            NEW(tmp2);
            String.Copy(tmp2^.name, insPtr^.name);
            tmp2^.next := tmp^.next;
            tmp^.next := tmp2;
            insPtr := insPtr^.next;
          END;
          EXIT;
        END;
        tmp := tmp^.next;
      END;
    END;
  END Replace;

  PROCEDURE GetNext(VAR l: List; VAR name: IdStr);
  BEGIN
    IF l.curr = NIL THEN
      l.curr := l.first;
    ELSE
      l.curr := l.curr^.next;
    END;
    IF l.curr = NIL THEN
      String.Copy(name, "");
    ELSE
      String.Copy(name, l.curr^.name);
    END;
  END GetNext;

  PROCEDURE Kill(VAR l: List);
    VAR
      old, tmp: IdPtr;
  BEGIN
    tmp := l.first;
    WHILE tmp # NIL DO
      old := tmp;
      tmp := tmp^.next;
      DISPOSE(old);
    END;
    l.first := NIL;
    l.last := NIL;
    l.curr := NIL;
  END Kill;

END Ids.
