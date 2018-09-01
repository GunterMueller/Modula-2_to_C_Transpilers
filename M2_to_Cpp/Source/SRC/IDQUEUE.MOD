IMPLEMENTATION MODULE IdQueue;

  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  FROM Defs IMPORT IdStr;
  IMPORT String; (* BMB *)

  PROCEDURE Init(VAR q: T);
  BEGIN
    q.first := NIL;
    q.last := NIL;
  END Init;

  PROCEDURE IsEmpty(q: T): BOOLEAN;
  BEGIN
    RETURN q.first = NIL;
  END IsEmpty;

  PROCEDURE AddRear(VAR q: T; s: IdStr);
    VAR
      tmp: PQueueElement;
  BEGIN
    NEW(tmp);
    String.Copy(tmp^.s, s);
    tmp^.next := NIL;
    IF IsEmpty(q) THEN
      q.first := tmp;
    ELSE
      q.last^.next := tmp;
    END;
    q.last := tmp;
  END AddRear;

  PROCEDURE First(q: T; VAR s: IdStr);
  BEGIN
    IF NOT IsEmpty(q) THEN
      String.Copy(s, q.first^.s);
    END;
  END First;

  PROCEDURE PopFirst(VAR q: T);
    VAR
      tmp: PQueueElement;
  BEGIN
    IF NOT IsEmpty(q) THEN
      tmp := q.first;
      q.first := q.first^.next;
      DISPOSE(tmp);
    END;
  END PopFirst;

  PROCEDURE Kill(VAR q: T);
    VAR
      old: PQueueElement;
  BEGIN
    WHILE q.first # NIL DO
      old := q.first;
      q.first := q.first^.next;
      DISPOSE(old);
    END;
  END Kill;

END IdQueue.
