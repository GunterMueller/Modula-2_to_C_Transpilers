IMPLEMENTATION MODULE Type;

  FROM Defs IMPORT IdStr, PIdStr;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT IdHash;

  TYPE
    ProcBlockStack = POINTER TO ProcBlockDesc;
    ProcBlockDesc = RECORD
      typeTable: IdHash.T;
      next: ProcBlockStack;
    END;
    ModBlockStack = POINTER TO ModBlockDesc;
    ModBlockDesc = RECORD
      pbs: ProcBlockStack;
      next: ModBlockStack;
    END;

  VAR
    mbs: ModBlockStack;
    builtInTypes: ProcBlockStack;
    p: PIdStr;

  PROCEDURE Init;
  BEGIN
    mbs := NIL;
    NEW(builtInTypes);
    builtInTypes^.next := NIL;
    IdHash.Init(builtInTypes^.typeTable);
    IdHash.Insert(builtInTypes^.typeTable, "CHAR", p);
    IdHash.Insert(builtInTypes^.typeTable, "INTEGER", p);
    IdHash.Insert(builtInTypes^.typeTable, "CARDINAL", p);
    IdHash.Insert(builtInTypes^.typeTable, "BOOLEAN", p);
    IdHash.Insert(builtInTypes^.typeTable, "REAL", p);
    IdHash.Insert(builtInTypes^.typeTable, "LONGREAL", p);
    IdHash.Insert(builtInTypes^.typeTable, "LONGINT", p);
    IdHash.Insert(builtInTypes^.typeTable, "LONGCARD", p);
    IdHash.Insert(builtInTypes^.typeTable, "SHORTINT", p);
    IdHash.Insert(builtInTypes^.typeTable, "SHORTCARD", p);
    IdHash.Insert(builtInTypes^.typeTable, "LOC", p);
    IdHash.Insert(builtInTypes^.typeTable, "WORD", p);
    IdHash.Insert(builtInTypes^.typeTable, "BYTE", p);
    IdHash.Insert(builtInTypes^.typeTable, "ADDRESS", p);
    IdHash.Insert(builtInTypes^.typeTable, "BITSET", p);
    IdHash.Insert(builtInTypes^.typeTable, "COMPLEX", p);
    IdHash.Insert(builtInTypes^.typeTable, "LONGCOMPLEX", p);
  END Init;

  PROCEDURE InitProc;
    VAR
      tmp: ProcBlockStack;
  BEGIN
    NEW(tmp);
    IdHash.Init(tmp^.typeTable);
    tmp^.next := mbs^.pbs;
    mbs^.pbs := tmp;
  END InitProc;

  PROCEDURE InitMod;
    VAR
      tmp: ModBlockStack;
  BEGIN
    NEW(tmp);
    tmp^.pbs := builtInTypes;
    tmp^.next := mbs;
    mbs := tmp;
    InitProc;
  END InitMod;

  PROCEDURE Define(typeId: IdStr);
  BEGIN
    IdHash.Insert(mbs^.pbs^.typeTable, typeId, p);
  END Define;

  PROCEDURE IsDefined(typeId: IdStr): BOOLEAN;
    VAR
      isDef: BOOLEAN;
      curr: ProcBlockStack;
  BEGIN
    isDef := FALSE;
    curr := mbs^.pbs;
    WHILE (curr # NIL) AND NOT isDef DO
      isDef := IdHash.IsMember(curr^.typeTable, typeId);
      curr := curr^.next;
    END;
    RETURN isDef;
  END IsDefined;

  PROCEDURE KillProc;
    VAR
      tmp: ProcBlockStack;
  BEGIN
    tmp := mbs^.pbs;
    IdHash.Kill(tmp^.typeTable);
    mbs^.pbs := tmp^.next;
    DISPOSE(tmp);
  END KillProc;

  PROCEDURE KillMod;
    VAR
      tmp: ModBlockStack;
  BEGIN
    KillProc;
    tmp := mbs;
    mbs := tmp^.next;
    DISPOSE(tmp);
  END KillMod;

  PROCEDURE Kill;
  BEGIN
    IdHash.Kill(builtInTypes^.typeTable);
    DISPOSE(builtInTypes);
  END Kill;

BEGIN
  Init;
END Type.
