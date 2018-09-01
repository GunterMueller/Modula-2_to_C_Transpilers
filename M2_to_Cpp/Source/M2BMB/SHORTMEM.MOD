(* ---------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library               *
 *                                                                  *
 * Implementation of:       ShortMem (1.1)                          *
 * Version:                 1.0                                     *
 * Date:                    January 23, 1997                        *
 * Conformance Test Passed: -                                       *
 * Platform:                80x86 / MS-DOS                          *
 * Requires:                TopSpeed Modula-2 ver. 1.x              *
 *                                                                  *
 * Portability:             This module is inherently non-portable  *
 *                          (if it is to be efficient). Every port  *
 *                          should provide its own module ShortMem  *
 * Author:                    Zoran Budimac                         *
 *                            University of Novi Sad, Yugoslavia    *
 *                            e-mail: zjb@unsim.ns.ac.yu            *
 * ---------------------------------------------------------------- *)

IMPLEMENTATION MODULE ShortMem;
IMPORT Lib;

PROCEDURE Copy(dest, source: ADDRESS; n: CARDINAL);
BEGIN
  Lib.Move(source, dest, n)
END Copy;

PROCEDURE Fill(source: ADDRESS; n: CARDINAL; val: BYTE);
BEGIN
  Lib.Fill(source, n, val)
END Fill;

PROCEDURE Equal(m1, m2: ADDRESS; n: CARDINAL): BOOLEAN;
BEGIN
  RETURN Lib.Compare(m1, m2, n) = 0
END Equal;

PROCEDURE LEq(m1, m2: ADDRESS; n: CARDINAL): BOOLEAN;
BEGIN
  RETURN Lib.Compare(m1, m2, n) <= 0
END LEq;

PROCEDURE GEq(m1, m2: ADDRESS; n: CARDINAL): BOOLEAN;
BEGIN
  RETURN Lib.Compare(m1, m2, n) >= 0
END GEq;

END ShortMem.
