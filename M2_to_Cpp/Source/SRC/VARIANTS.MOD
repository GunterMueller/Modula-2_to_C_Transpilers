IMPLEMENTATION MODULE Variants;

  FROM Defs IMPORT IdStr, BigStr;
  IMPORT Ids, ConstStr;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT String;

  PROCEDURE Init(VAR l: List; casePath: BigStr);
  BEGIN
    String.Copy(l.casePath, casePath);
    l.first := NIL;
    l.last := NIL;
    l.curr := NIL;
  END Init;

  PROCEDURE InsertType(VAR l: List; type: ARRAY OF CHAR);
    VAR
      tmp: VariantsPtr;
  BEGIN
    NEW(tmp);
    Ids.Init(tmp^.nameList);
    ConstStr.Init(tmp^.type, type);
    tmp^.next := NIL;
    IF l.first = NIL THEN
      l.first := tmp;
    ELSE
      l.last^.next := tmp;
    END;
    l.last := tmp;
  END InsertType;

  PROCEDURE InsertName(VAR l: List; name: IdStr);
  BEGIN
    IF l.last # NIL THEN
      Ids.Insert(l.last^.nameList, name);
    END;
  END InsertName;

  PROCEDURE GetCasePath(l: List; VAR casePath: BigStr);
  BEGIN
    String.Copy(casePath, l.casePath);
  END GetCasePath;

  PROCEDURE GetNextType(VAR l: List; VAR type: ARRAY OF CHAR);
  BEGIN
    IF l.curr = NIL THEN
      l.curr := l.first;
    ELSE
      l.curr := l.curr^.next;
    END;
    IF l.curr = NIL THEN
      String.Copy(type, "");
    ELSE
      ConstStr.ToStr(type, l.curr^.type);
    END;
  END GetNextType;

  PROCEDURE GetNextName(VAR l: List; VAR name: IdStr);
  BEGIN
    IF l.curr = NIL THEN
      String.Copy(name, "");
    ELSE
      Ids.GetNext(l.curr^.nameList, name);
    END;
  END GetNextName;

  PROCEDURE IsEmpty(l: List): BOOLEAN;
  BEGIN
    RETURN l.first = NIL;
  END IsEmpty;

  PROCEDURE Kill(VAR l: List);
    VAR
      old, tmp: VariantsPtr;
  BEGIN
    tmp := l.first;
    WHILE tmp # NIL DO
      old := tmp;
      tmp := tmp^.next;
      Ids.Kill(old^.nameList);
      ConstStr.Kill(old^.type);
      DISPOSE(old);
    END;
    l.first := NIL;
    l.last := NIL;
    l.curr := NIL;
  END Kill;

END Variants.
