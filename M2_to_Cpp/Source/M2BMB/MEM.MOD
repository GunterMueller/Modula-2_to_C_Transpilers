(* ------------------------------------------------------------------ *
 *                BMB Modula-2 Cross-Platform Library                 *
 *                                                                    *
 * Implementation of:       Mem (1.1)                                 *
 * Version:                 1.0                                       *
 * Date:                    March 8, 1995                             *
 * Conformance Test Passed: -                                         *
 * Platform:                80x86 / MS-DOS                            *
 * Requires:                TopSpeed Modula-2 Compiler ver 1.x        *
 *                                                                    *
 * Portability:             This module is non-portable.              *
 * Author:                    Daniel Sendula                          *
 *                            University of Novi Sad, Yugoslavia      *
 *                            e-mail: isendula@unsim.ns.ac.yu         *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE Mem;
IMPORT Lib, ShortMem;

CONST
   MAXCARD = MAX(CARDINAL);
   MAXCARDDIV2 = MAXCARD DIV 2;

PROCEDURE Copy(dest, source: ADDRESS; n: LONGCARD);
VAR
  m: LONGCARD;
BEGIN
  m := n DIV 2;
  IF m = 0 THEN
    n := m;
    WHILE n > MAXCARD DO
      Lib.WordMove(source, dest, MAXCARDDIV2);
      DEC(n, MAXCARD);
    END;
    IF n > 0 THEN
      Lib.WordMove(source, dest, CARDINAL(n))
    END;
  ELSE
    WHILE n > MAXCARD DO
      Lib.WordMove(source, dest, MAXCARDDIV2);
      DEC(n, MAXCARD);
    END;
    IF n > 0 THEN
      ShortMem.Copy(dest, source, CARDINAL(n))
    END;
  END;
END Copy;

PROCEDURE Fill(source: ADDRESS; n: LONGCARD; val: BYTE);
VAR
  m: LONGCARD;
  valw: CARDINAL;
BEGIN
  m := n DIV 2;
  valw := (CARDINAL(val) << 8) + CARDINAL(val);
  IF m = 0 THEN
    n := m;
    WHILE n > MAXCARD DO
      Lib.WordFill(source, MAXCARD, valw);
      DEC(n, MAXCARD);
    END;
    IF n > 0 THEN
      Lib.WordFill(source, CARDINAL(n), valw)
    END;
  ELSE
    WHILE n > MAXCARD DO
      Lib.WordFill(source, MAXCARDDIV2, valw);
      DEC(n, MAXCARD);
    END;
    IF n > 0 THEN
      ShortMem.Fill(source, CARDINAL(n), val)
    END;
  END;
END Fill;

PROCEDURE Equal(m1, m2: ADDRESS; n: LONGCARD): BOOLEAN;
BEGIN
  WHILE n > MAXCARD DO
    IF NOT ShortMem.Equal(m1, m2, MAXCARDDIV2) THEN
      RETURN FALSE
    END;
    DEC(n, MAXCARD);
  END;
  IF n > 0 THEN
    RETURN ShortMem.Equal(m1, m2, CARDINAL(n))
  END;
END Equal;

PROCEDURE LEq(m1, m2: ADDRESS; n: LONGCARD): BOOLEAN;
BEGIN
  WHILE n > MAXCARD DO
    IF Lib.Compare(m1, m2, MAXCARDDIV2) > 0 THEN
      RETURN FALSE
    END;
    DEC(n, MAXCARD);
  END;
  IF n > 0 THEN
    RETURN ShortMem.LEq(m1, m2, CARDINAL(n))
  END;
END LEq;

PROCEDURE GEq(m1, m2: ADDRESS; n: LONGCARD): BOOLEAN;
BEGIN
  WHILE n > MAXCARD DO
    IF Lib.Compare(m1, m2, MAXCARDDIV2) < 0 THEN
      RETURN FALSE
    END;
    DEC(n, MAXCARD);
  END;
  IF n > 0 THEN
    RETURN ShortMem.GEq(m1, m2, CARDINAL(n))
  END;
END GEq;

END Mem.
