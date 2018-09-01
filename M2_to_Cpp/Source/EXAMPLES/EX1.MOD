(* By Milos Radovanovic, 2001 *)

MODULE Ex1;
(* This example doesn't do anything, it just demonstrates how variant records
   and procedures with {ARRAY OF} BYTE parameters are tranlsated to C++.
   For a demonstration of how other M2 features are translated,
   see the SRC directory *)

(* Note: JPI 1.12 crashes when trying to compile this. TS 3.02 works fine.
   I guess it's a bug in JPI. Anyway, the purpose of this example is to
   be translated to C++, not compiled and run (obviously :) *)

  IMPORT SYSTEM;

  TYPE
    VariantRecord = RECORD
      i1, i2: INTEGER;
      CASE sel1: CHAR OF
        'A'..'Z':
          r1, r2: REAL;
      | 'a'..'z':
          n1, n2, n3: CARDINAL;
        ELSE
          CASE sel2: CARDINAL OF
            1..100:
              b1, b2: BITSET;
          | 101..1000, 2000, 3000:
              l: LONGCARD;
          END;
      END;
      CASE : BOOLEAN OF
        FALSE:
          i: INTEGER;
      | TRUE:
          b: BITSET;
      END;
      a: SYSTEM.ADDRESS;
    END;

  PROCEDURE Proc(b: BYTE; ab: ARRAY OF BYTE; i: INTEGER);
  BEGIN
  END Proc;

BEGIN
END Ex1.
