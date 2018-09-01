IMPLEMENTATION MODULE VFields;

  IMPORT Variants;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;

  TYPE
    VFieldsPtr = POINTER TO VFieldsDesc;
    VFieldsDesc = RECORD
      vList: Variants.List;
      next: VFieldsPtr;
    END;
    List = RECORD
      first, last, curr: VFieldsPtr;
    END;

  VAR
    l: List;
    
  PROCEDURE Init;
  BEGIN
    l.first := NIL;
    l.last := NIL;
    l.curr := NIL;
  END Init;

  PROCEDURE Insert(vList: Variants.List);
    VAR
      tmp: VFieldsPtr;
  BEGIN
    NEW(tmp);
    tmp^.vList := vList;
    tmp^.next := NIL;
    IF l.first = NIL THEN
      l.first := tmp;
    ELSE
      l.last^.next := tmp;
    END;
    l.last := tmp;
  END Insert;

  PROCEDURE GetNext(VAR vList: Variants.List);
  BEGIN
    IF l.curr = NIL THEN
      l.curr := l.first;
    ELSE
      l.curr := l.curr^.next;
    END;
    IF l.curr = NIL THEN
      Variants.Init(vList, "");
    ELSE
      vList := l.curr^.vList;
    END;
  END GetNext;

  PROCEDURE IsEmpty(): BOOLEAN;
  BEGIN
    RETURN l.first = NIL;
  END IsEmpty;

  PROCEDURE Kill;
    VAR
      old, tmp: VFieldsPtr;
  BEGIN
    tmp := l.first;
    WHILE tmp # NIL DO
      old := tmp;
      tmp := tmp^.next;
      Variants.Kill(old^.vList);
      DISPOSE(old);
    END;
    l.first := NIL;
    l.last := NIL;
    l.curr := NIL;
  END Kill;

BEGIN
  Init;
END VFields.
