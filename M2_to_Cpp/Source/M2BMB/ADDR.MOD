(* ---------------------------------------------------------------- *
 *                  BMB Modula-2 Cross Platform Library             *
 *                                                                  *
 * Implementation of:       Addr (1.1)                              *
 * Version:                 1.0                                     *
 * Date:                    April 6, 1996                           *
 * Conformance Test Passed: -                                       *
 * Platform:                80x86 / MS-DOS                          *
 * Requires:                TopSpeed Modula-2 ver. 1.x              *
 * Portability:             This module is inherently non-portable  *
 *                          (if it is to be efficient). Every port  *
 *                          should provide its own module Addr      *
 *                                                                  *
 * Author:                    Daniel Sendula                        *
 *                            University of Novi Sad, Yugoslavia    *
 *                            e-mail: zjb@unsim.ns.ac.yu            *
 * ---------------------------------------------------------------- *)

IMPLEMENTATION MODULE Addr;
IMPORT Lib;

CONST
  MAXCARD = MAX(CARDINAL);

  PROCEDURE Inc(VAR a: ADDRESS; n: LONGCARD);
  BEGIN
    WHILE n > MAXCARD DO
      Lib.IncAddr(a, MAXCARD);
      DEC(n, MAXCARD);
    END;
    IF n > 0 THEN
       Lib.IncAddr(a, CARDINAL(n))
    END;
  END Inc;

  PROCEDURE Dec(VAR a: ADDRESS; n: LONGCARD);
  BEGIN
    WHILE n > MAXCARD DO
      Lib.DecAddr(a, MAXCARD);
      DEC(n, MAXCARD);
    END;
    IF n > 0 THEN
      Lib.DecAddr(a, CARDINAL(n))
    END;
  END Dec;

END Addr.

