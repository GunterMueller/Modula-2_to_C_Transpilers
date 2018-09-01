(* p.156 9b-7b *)

(* Compilation units *)

(* Procedures,operating on operands of the opaque exported type,   *)
(*   must be defined in the same implementation module which hides *)
(*   the type's properties.                                        *)

(* The assignment  ptr^.int:=4  is illegal                         *)

MODULE test14f;
 IMPORT add14f,Storage;
 VAR
    ptr: add14f.PtrType;
 BEGIN
  IF Storage.Available(SIZE(add14f.Type)) THEN
     Storage.ALLOCATE(ptr,SIZE(add14f.Type));
     ptr^.int:=4;
  END;
 END test14f.
