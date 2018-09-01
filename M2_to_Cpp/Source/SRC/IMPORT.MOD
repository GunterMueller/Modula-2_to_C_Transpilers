IMPLEMENTATION MODULE Import;

  FROM Defs IMPORT IdStr, PIdStr;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT IdQueue, IdHash;

  TYPE
    ImportPtr  = POINTER TO ImportDesc;
    ImportDesc = RECORD
      moduleId: PIdStr;
      importedIds: IdQueue.T;
      next: ImportPtr;
    END;
    BlockStack = POINTER TO BlockDesc;
    BlockDesc = RECORD
      first, last, curr: ImportPtr;
      modTable: IdHash.T;
      next: BlockStack;
    END;

  VAR
    bs: BlockStack;

  PROCEDURE Init;
    VAR
      tmp: BlockStack;
  BEGIN
    NEW(tmp);
    tmp^.first := NIL;
    tmp^.last := NIL;
    tmp^.curr := NIL;
    IdHash.Init(tmp^.modTable);
    tmp^.next := bs;
    bs := tmp;
  END Init;

  PROCEDURE AddModule(VAR s: IdStr);
    VAR
      tmp: ImportPtr;
  BEGIN
    NEW(tmp);
    tmp^.next := NIL;
    IdQueue.Init(tmp^.importedIds);
    IdHash.Insert(bs^.modTable, s, tmp^.moduleId);
    IF bs^.last = NIL THEN
      bs^.first := tmp;
    ELSE
      bs^.last^.next := tmp;
    END;
    bs^.last := tmp;
  END AddModule;

  PROCEDURE AddImported(VAR s: IdStr);
  BEGIN
    IdQueue.AddRear(bs^.last^.importedIds, s);
  END AddImported;

  PROCEDURE NextModule(VAR s: IdStr);
  BEGIN
    IF bs^.curr # NIL THEN
      bs^.curr := bs^.curr^.next;
      IF bs^.curr = NIL THEN
        s := "";
      ELSE
        s := bs^.curr^.moduleId^;
      END;
    ELSE
      bs^.curr := bs^.first;
      IF bs^.curr # NIL THEN
        s := bs^.curr^.moduleId^;
      ELSE
        s := "";
      END;
    END;
  END NextModule;

  PROCEDURE NextImported(VAR s: IdStr);
  BEGIN
    IF IdQueue.IsEmpty(bs^.curr^.importedIds) THEN
      s := "";
    ELSE
      IdQueue.First(bs^.curr^.importedIds, s);
      IdQueue.PopFirst(bs^.curr^.importedIds);
    END;
  END NextImported;

  PROCEDURE IsQualified(): BOOLEAN;
  BEGIN
    RETURN IdQueue.IsEmpty(bs^.curr^.importedIds);
  END IsQualified;

  PROCEDURE HasModule(VAR s: IdStr): BOOLEAN;
  BEGIN
    RETURN IdHash.IsMember(bs^.modTable, s);
  END HasModule;

  PROCEDURE Kill;
    VAR
      tmp: BlockStack;
  BEGIN
    IdHash.Kill(bs^.modTable);
    WHILE bs^.first # NIL DO
      IdQueue.Kill(bs^.first^.importedIds);
      bs^.first := bs^.first^.next;
    END;
    bs^.last := NIL;
    bs^.curr := NIL;
    tmp := bs;
    bs := bs^.next;
    DISPOSE(tmp);
  END Kill;

BEGIN
  bs := NIL;
END Import.
