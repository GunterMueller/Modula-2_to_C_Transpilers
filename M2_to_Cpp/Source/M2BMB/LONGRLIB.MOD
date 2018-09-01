(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       LongRLib (1.1)                            *
 * Version:                 1.0                                       *
 * Date:                    February 27, 1995                         *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * Portability:             This module is inherently non-portable    *
 *                          (if it is to be efficient). Every port    *
 *                          should provide its own module LongRLib    *
 *                                                                    *
 * Author:                      Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE LongRLib;
FROM MATHLIB IMPORT MathError;   (* TopSpeed mathematical operations *)
IMPORT Wr;

VAR errorCode : CARDINAL;

PROCEDURE ATanH(r: LONGREAL): LONGREAL;
BEGIN
  (* ????? *)
  RETURN 0.0
END ATanH;

PROCEDURE MyMathError(x: LONGREAL; s: ARRAY OF CHAR);
   (* Exception handling procedure for dealing with floating point errors *)
BEGIN
  errorCode := arithmeticError;
END MyMathError;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** LongRLib.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    arithmeticError   : Wr.Str('Floating point error')
  END;
  HALT;
END PrintError;

PROCEDURE Error(): CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c
END Error;

BEGIN
  errorCode := noError;
  errorHandler := PrintError;
  MathError := MyMathError;
      (* Initialzaition of exception handling procedure *)
END LongRLib.