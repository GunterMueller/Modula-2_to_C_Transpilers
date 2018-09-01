IMPLEMENTATION MODULE Table;
 FROM InOut IMPORT Write,WriteLn,WriteInt;
 FROM Storage IMPORT ALLOCATE;

 CONST TableLong = 3000;
 TYPE TreePtr= POINTER TO Word;
      ListPtr = POINTER TO Element;
      Element = RECORD numb: INTEGER;
	    next: ListPtr
		END;
   Word = RECORD Key: INTEGER; (*table index*)
     first: ListPtr; (*list head*)
     left,right: TreePtr;
    END;
  Table = TreePtr;

VAR ident: ARRAY[0..WordLong] OF CHAR;
 TableIndex: INTEGER;
 ChTab: ARRAY[0..TableLong-1] OF CHAR;

 PROCEDURE Initiate(VAR t: Table);
 BEGIN ALLOCATE(t,SIZE(Word)); t^.right :=NIL;
 END Initiate;

 PROCEDURE Search(p: TreePtr): TreePtr;
  (*search of node with name ident*)
  TYPE Relation = (lt,eq,gt);
  VAR q: TreePtr;
    r: Relation; i: INTEGER;

   PROCEDURE comp(k: INTEGER): Relation;
   (*compare ident and ChTab[k]*)
    VAR i: INTEGER;
     R: Relation; x,y: CHAR;
    BEGIN i:=0; R := eq;
     LOOP x := ident[i]; y := ChTab[k];
      IF CAP(x) # CAP(y) THEN EXIT END;
      IF x <= " " THEN RETURN R END;
      IF x < y THEN R := lt;
      ELSIF x> y THEN R := gt;
      END;
      i := i + 1; k := k + 1
     END;
     IF CAP(x)> CAP(y) THEN RETURN gt
     ELSE RETURN lt
     END
    END comp;

 BEGIN q := p^.right; r := gt;
  WHILE q # NIL DO
   p := q;
   r := comp(p^.Key);
   IF r = eq THEN RETURN p;
   ELSIF r = lt THEN q := p^.left
   ELSE q := p^.right
   END
  END;
  ALLOCATE(q,SIZE(Word)); (* node is not found: insertion*)
  IF q # NIL THEN
   WITH q^ DO
    Key := TableIndex; first := NIL;
    left := NIL; right := NIL
   END;
   IF r = lt THEN p^.left := q;
   ELSE p^.right := q
   END;
   i := 0;
   (* copy idetifier to table ChTab*)
   WHILE ident[i] > " " DO
    IF TableIndex = TableLong THEN
     ChTab[TableIndex] := " "; ident[i] := " ";
     overfull := 1
    ELSE ChTab[TableIndex] := ident[i];
     TableIndex := TableIndex + 1; i := i + 1
    END
   END;
   ChTab[TableIndex] := " ";
   TableIndex := TableIndex +1
  END;
  RETURN q
 END Search;

 PROCEDURE Add(t: Table; VAR x: ARRAY OF CHAR; n: INTEGER);
  VAR p: TreePtr; q: ListPtr; i: INTEGER;
  BEGIN i := 0;
    REPEAT ident[i] := x[i]; i := i + 1
    UNTIL (ident[i-1] = " ") OR (i = WordLong);
    p := Search(t);
    IF p = NIL THEN overfull := 2
    ELSE ALLOCATE(q,SIZE(Element));
     IF q = NIL THEN overfull := 3 ELSE
      q^.numb := n; q^.next := p^.first;
      p^.first := q
     END
    END
  END Add;

 PROCEDURE Print(t: Table);

  PROCEDURE PrElem(p: TreePtr);
   CONST L = 6;
    N = (LineWidth-WordLong) DIV L;
   VAR ch: CHAR;
    i,k: INTEGER; q: ListPtr;
   BEGIN i := WordLong + 1; k := p^.Key;
    REPEAT ch := ChTab[k];
     i := i -1; k := k+1; Write(ch)
    UNTIL ch <= " ";
    WHILE i >0 DO
     Write(" "); i := i -1
    END;
    q := p^.first; i := N;
    WHILE q # NIL DO
     IF i =0 THEN
      WriteLn; i := WordLong + 1;
      REPEAT Write(" "); i := i -1
      UNTIL i =0;
      i := N
     END;
     WriteInt(q^.numb,L);
     q := q^.next;
     i := i -1
    END;
    WriteLn
   END PrElem;

  PROCEDURE TreeWalk(p: TreePtr);
  BEGIN
   IF p # NIL THEN
    TreeWalk(p^.left); PrElem(p); TreeWalk(p^.right);
   END
  END TreeWalk;

 BEGIN WriteLn;
  TreeWalk(t^.right)
 END Print;

BEGIN
 TableIndex:= 0; ident[WordLong] := " ";
 overfull := 0
END Table.
