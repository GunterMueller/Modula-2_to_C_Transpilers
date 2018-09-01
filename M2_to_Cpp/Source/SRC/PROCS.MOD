IMPLEMENTATION MODULE Procs;

  FROM Defs IMPORT IdStr, BigStr;
  FROM Storage IMPORT ALLOCATE, DEALLOCATE;
  IMPORT String, Fmt; (* BMB *)

  PROCEDURE Init(VAR p: ProcDesc);
  BEGIN
    p.templateNo := 0;
    p.first := NIL;
    p.last := NIL;
    p.curr := NIL;
  END Init;

  PROCEDURE AddParam(VAR p: ProcDesc;
                     VAR name, type: ARRAY OF CHAR;
                         arrayLvl: CARDINAL;
                         isVar: BOOLEAN);
    VAR
      tmp, iter: ParamList;
      cardStr: ARRAY [0..15] OF CHAR;
      i: CARDINAL;
  BEGIN
    NEW(tmp);
    String.Copy(tmp^.name, name);
    String.Copy(tmp^.type, type);
    tmp^.arrayLvl := arrayLvl;
    tmp^.isVar := isVar;
    tmp^.isTemplate := FALSE;
    tmp^.next := NIL;
    tmp^.prev := NIL;
    IF String.Equal(type, "LOC")  OR
       String.Equal(type, "BYTE") OR
       String.Equal(type, "WORD")
    THEN
      INC(p.templateNo);
      tmp^.isTemplate := TRUE;
    (*FOR i := 1 TO arrayLvl DO
        String.Insert(tmp^.type, "ARRAYOF_", 0);
      END;
      String.Insert(tmp^.type, "_", 0);
      Fmt.Card(cardStr, p.templateNo);
      String.Append(tmp^.type, cardStr);*)
    END;
    IF p.last = NIL THEN
      p.first := tmp;
    ELSE
      p.last^.next := tmp;
      tmp^.prev := p.last;
    END;
    p.last := tmp;
    IF NOT String.Equal(tmp^.type, "") THEN
      iter := tmp^.prev;
      LOOP
        IF iter = NIL THEN EXIT END;
        IF NOT String.Equal(iter^.type, "") THEN EXIT END;
        String.Copy(iter^.type, type);
        iter^.arrayLvl := arrayLvl;
        iter^.isTemplate := tmp^.isTemplate;
        iter := iter^.prev;
      END;
    END;
  END AddParam;

  PROCEDURE IsTemplate(p: ProcDesc): BOOLEAN;
  BEGIN
    RETURN p.templateNo > 0;
  END IsTemplate;

  PROCEDURE GetNextTemplateParam(VAR p: ProcDesc;
                                 VAR name, type: ARRAY OF CHAR;
                                 VAR arrayLvl: CARDINAL;
                                 VAR isVar: BOOLEAN);
  BEGIN
    IF p.curr = NIL THEN
      p.curr := p.first;
    ELSE
      p.curr := p.curr^.next;
    END;
    LOOP
      IF p.curr = NIL THEN EXIT END;
      IF p.curr^.isTemplate THEN EXIT END;
      p.curr := p.curr^.next;
    END;
    IF p.curr # NIL THEN
      String.Copy(name, p.curr^.name);
      String.Copy(type, p.curr^.type);
      arrayLvl := p.curr^.arrayLvl;
      isVar := p.curr^.isVar;
    ELSE
      String.Copy(name, "");
      String.Copy(type, "");
      arrayLvl := 0;
      isVar := FALSE;
    END;
  END GetNextTemplateParam;

  PROCEDURE GetNextParam(VAR p: ProcDesc;
                         VAR name, type: ARRAY OF CHAR;
                         VAR arrayLvl: CARDINAL;
                         VAR isVar, isTemplate: BOOLEAN);
  BEGIN
    IF p.curr = NIL THEN
      p.curr := p.first;
    ELSE
      p.curr := p.curr^.next;
    END;
    IF p.curr # NIL THEN
      String.Copy(name, p.curr^.name);
      String.Copy(type, p.curr^.type);
      arrayLvl := p.curr^.arrayLvl;
      isVar := p.curr^.isVar;
      isTemplate := p.curr^.isTemplate;
    ELSE
      String.Copy(name, "");
      String.Copy(type, "");
      arrayLvl := 0;
      isVar := FALSE;
      isTemplate := FALSE;
    END;
  END GetNextParam;

  PROCEDURE Kill(VAR p: ProcDesc);
    VAR
      old: ParamList;
  BEGIN
    WHILE p.first # NIL DO
      old := p.first;
      p.first := p.first^.next;
      DISPOSE(old);
    END;
  END Kill;

BEGIN
  level := 0;
END Procs.
